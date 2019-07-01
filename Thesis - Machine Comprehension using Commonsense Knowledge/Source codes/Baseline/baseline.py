"""
File: baseline.py
Author: Tomáš Daniš
Login: xdanis05
Description: Main module of the project used for training/evaluating the stanford attentive reader baseline
"""
import mcscript_reader as mcread
import model_ops
import attentive_reader

import torchtext.data as torch_data
import torch

import json
import sys
import os
import pickle


def save_model(path, filename, net, config, config_filename='config.json'):
    """
    Saves an existing model into a file along with its configuration

    :param path: Directory to which to save the model
    :param filename: Name of the file the model's weights should be saved to
    :param net: Instance of a PyTorch module
    :param config: Configuration of the model to save
    :param config_filename: Name of the file to which to save the model configuration
    :return: None
    """
    os.makedirs(path, exist_ok=True)
    torch.save(net.cpu().state_dict(), os.path.join(path, filename))
    fp = open(os.path.join(path, config_filename), mode='w')
    json.dump(config, fp)
    fp.close()


def save_vocab(vocab, path, vocab_file='vocab.vt'):
    """
    Saves the vocabulary of a model.

    :param vocab: The vocabulary instance, must be pickleable
    :param path: Directory to which to save the vocabulary
    :param vocab_file: Name of the file to which to save the vocabulary to
    :return: None
    """
    os.makedirs(path, exist_ok=True)
    with open(os.path.join(path, vocab_file), mode="wb") as fp:
        pickle.dump(vocab, fp)


def load_model(config, model_cls, vocab):
    """
    Load a model previously saved by save_model

    :param config: Configuration of the model saved along with it
    :param model_cls: Class of the model that is used to instantiate it
    :param vocab: Vocabulary of the model constructed during trainign
    :return: Instance of model_cls representing the loaded model
    """
    net = model_cls(dict_size=len(vocab), **config)
    net.load_state_dict(torch.load(config['weights']))
    return net


def load_vocab(path, vocab_file='vocab.vt'):
    """
    Load a vocabulary previously saved with save_vocab

    :param path: Directory where to find the vocabulary file
    :param vocab_file: The name of the vocabulary file
    :return: Unpickled vocabulary instance
    """
    with open(os.path.join(path, vocab_file), mode="rb") as fp:
        return pickle.load(fp)


def load_model_config(path):
    """
    Loads a saved model configuration from a JSON file

    :param path: Path to the configuration file
    :return: Dictionary/List representing the parsed JSON file
    """
    fp = open(path, mode='r')
    config = json.load(fp)
    return config


def init_emb(vocab, num_special_toks=2):
    """
    Initializes unknown embeddings to random values sampled from a uniform distribution
    :param vocab: Vocabulary containing all the words and known embeddings.
    :param num_special_toks: Number of special tokens whose vectors should be kept to zero
    :return: None
    """
    emb_vectors = vocab.vectors
    sweep_range = len(vocab)
    for i in range(num_special_toks, sweep_range):
        if len(emb_vectors[i, :].nonzero()) == 0:
            emb_vectors[i].data = torch.nn.init.uniform_(emb_vectors[i])


def totext(batch, vocab, batch_first=True, remove_specials=True, check_for_zero_vectors=True, pad_token='<pad>',
           sep=" "):
    """
    Debugging function used for development
    """
    textlist = []
    if not batch_first:
        batch = batch.transpose(0, 1)
    for ex in batch:
        if remove_specials:
            textlist.append(
                sep.join(
                    [vocab.itos[ix.item() if hasattr(ix, "item") else ix] for ix in ex
                     if ix != vocab.stoi[pad_token]]))
        else:
            if check_for_zero_vectors:
                text = []
                for ix in ex:
                    text.append(vocab.itos[ix.item() if hasattr(ix, "item") else ix])
                textlist.append(sep.join(text))
            else:
                textlist.append(sep.join([vocab.itos[ix.item() if hasattr(ix, "item") else ix] for ix in ex]))
    return textlist


if __name__ == "__main__":
    device = "cuda:0" if torch.cuda.is_available() else "cpu"
    config = load_model_config(sys.argv[1])
    torch.manual_seed(42)
    torch.cuda.manual_seed(42)

    # Dataset format
    text_field = torch_data.ReversibleField(sequential=True, lower=True, use_vocab=True, include_lengths=True,
                                            fix_length=256, tokenize="spacy")
    label_field = torch_data.Field(sequential=False, use_vocab=False, is_target=True)
    example_template = [('document', text_field),
                        ('question', text_field),
                        ('answer1', text_field),
                        ('answer2', text_field),
                        ('correct', label_field)]

    if sys.argv[2] == "train":
        # Read dataset
        mcscript_train, mcscript_dev, mcscript_val = mcread.read_mcscript(config["dataset_dir"], example_template)

        # Construct vocalbulary
        text_field.build_vocab(mcscript_train, vectors="glove.6B.{0}d".format(config['embed_size']))
        train_iter = torch_data.Iterator(mcscript_train, device=device, batch_size=config["batch_size"],
                                         sort_key=lambda x: len(x.document))
        dev_iter = torch_data.Iterator(mcscript_dev, device=device, batch_size=config["batch_size"], sort_key=lambda x: len(x.document))
        val_iter = torch_data.Iterator(mcscript_val, device=device, batch_size=config["batch_size"], sort_key=lambda x: len(x.document))

        # Initialize the network
        net = attentive_reader.AttReader(len(text_field.vocab.vectors))
        net.load_embeddings(text_field.vocab.vectors)
        net.embedding.weight.requires_grad = False # We do not want to retrain word embeddings
        # for p in filter(lambda p: p.requires_grad, net.parameters()):
        #     if p.dim() > 1:
        #         torch.nn.init.xavier_uniform_(p)
        net = net.to(device)

        # Train the model on the dataset
        model_ops.train(net, train_iter, val_iter, vocabs=(text_field.vocab,), epoch_num=config['epoch_num'],
                        lr=config['learning_rate'])

        # Save the trained model along with its vocabulary
        save_model(config["model_path"], config["weights"], net, config, sys.argv[1])
        save_vocab(text_field.vocab, config["model_path"])

        # Evaluate the performance of the model on the test set and report the results
        accuracy, f1s = model_ops.eval_model(net, val_iter)
        print("Accuracy: {0}%".format(accuracy))
        for i, f1 in enumerate(f1s):
            print("Answer {0} F1: {1}".format(i, f1))

    elif sys.argv[2] == 'eval':
        # Load the test set for evaluation
        mcscript_train, mcscript_dev, mcscript_val = mcread.read_mcscript(config["dataset_dir"], example_template,
                                                                          train_suffix=None, dev_suffix=None)
        # Load the model
        text_field.vocab = load_vocab(config["model_path"])
        net = load_model(config, attentive_reader.AttReader, text_field.vocab)
        net = net.to(device)

        # Prepare data
        val_iter = torch_data.Iterator(mcscript_val, device=device, batch_size=config["batch_size"], sort_key=lambda x: len(x.document))

        # Evaluate the loaded model
        accuracy, f1s = model_ops.eval_model(net, val_iter)
        print("Accuracy: {0}%".format(accuracy))
        for i, f1 in enumerate(f1s):
            print("Answer {0} F1: {1}".format(i, f1))
