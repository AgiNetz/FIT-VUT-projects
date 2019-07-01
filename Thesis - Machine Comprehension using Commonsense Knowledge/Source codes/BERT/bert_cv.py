"""
File: bert_cv.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module of a BERT architecture with an implemented interface for easy use.
"""
from pytorch_pretrained_bert.file_utils import PYTORCH_PRETRAINED_BERT_CACHE
from pytorch_pretrained_bert.modeling import BertConfig
from pytorch_pretrained_bert.optimization import BertAdam
from pytorch_pretrained_bert.tokenization import BertTokenizer

import numpy as np
import torch
from torch.utils.data import (DataLoader, RandomSampler, SequentialSampler,
                              TensorDataset)
from torch.utils.data.distributed import DistributedSampler
from tqdm import tqdm, trange
import sklearn.metrics as mt

import os
from collections import Counter
import copy
import evaluation as ev


class BERTwithCV:
    """
    Class representing  a BERT model. Methods for training, training with cross validation, evaluation and prediction
    are implemented to allow usage out-of-the-box easily.
    """
    def __init__(self, model_name, model_cls, cv, load_from_finetuned=None, finetuned_cls=None,
                 finetuned_config='config.json', finetuned_file='model.wt', finetuned_params={},
                 model_params={}, local_rank=-1, no_cuda=False, load_pretrained=True):
        """
        Construct a BERT model with given parameters

        :param model_name: Name of the pre-trained model from the pytorch_pretrained_bert library.
        :param model_cls: Underlying BERT model class from the pytorch_pretrained_bert library.
        :param cv: Number of split to use with in cross validation.
        :param load_from_finetuned: If given, loads the weights of the main BERT module from a previously finetuned
        saved model. This parameter contains the directory saved model is. Note that such a model still needs to be
        trained as this only loads the weights of the BERT module, not the subsequent classifier layers.
        :param finetuned_cls: Class of the finetuned model to load. This class should be a BERT model from the
        pytorch_pretrained_bert library. None if load_from_finetuned is None
        :param finetuned_config: Config of the finetuned model to load. None if load_from_finetuned is None.
        :param finetuned_file: Name of the file containing the weights of the model to load
        :param finetuned_params: A dictionary containing any parameters to pass to the constructor of finetuned_cls
        :param model_params: A dictionary containing any parameters to pass to the constructor of model_cls
        :param local_rank: Local rank for distributed training on GPU
        :param no_cuda: Whether to use CUDA for training or not
        :param load_pretrained: Whether to load pre-trained BERT weights or initialize them randomly
        """
        self.model_name = model_name
        self.model_cls = model_cls
        self.cv = cv
        self.model_params = model_params
        self.do_lower_case = model_name.endswith('uncased')
        self.tokenizer = BertTokenizer.from_pretrained(self.model_name, do_lower_case=self.do_lower_case)
        self.local_rank = local_rank
        self.no_cuda = no_cuda

        if self.local_rank == -1 or self.no_cuda:
            self.device = torch.device("cuda" if torch.cuda.is_available() and not self.no_cuda else "cpu")
            self.n_gpu = torch.cuda.device_count()
        else:
            torch.cuda.set_device(self.local_rank)
            self.device = torch.device("cuda", self.local_rank)
            self.n_gpu = 1
            # Initializes the distributed backend which will take care of sychronizing nodes/GPUs
            torch.distributed.init_process_group(backend='nccl')

        # Prepare model
        self.model = self.model_cls.from_pretrained(self.model_name,
                                                    cache_dir=os.path.join(str(PYTORCH_PRETRAINED_BERT_CACHE),
                                                                           'distributed_{}'.format(self.local_rank)),
                                                    **self.model_params)
        if not load_pretrained:
            self.model.apply(self.model.init_bert_weights)

        if load_from_finetuned:
            self._init_from_finetuned(load_from_finetuned, finetuned_cls,
                                      finetuned_config, finetuned_file, **finetuned_params)

        self.starting_model = self.model  # Keep a copy of the untrained model to restore it during CV training
        self.model = copy.deepcopy(self.starting_model)

    def save_model(self, output_dir, config_file='config.json', model_file='model.wt'):
        """
        Save the model into a file

        :param output_dir: Directory to which to save the model
        :param config_file: Name of the file to which to save the configuration of the model
        :param model_file: Name of the file to which to save the weights of the model
        :return:
        """
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        model_to_save = self.model.module if hasattr(self.model, 'module') else self.model
        output_model_file = os.path.join(output_dir, model_file)
        torch.save(model_to_save.state_dict(), output_model_file)
        output_model_config = os.path.join(output_dir, config_file)
        with open(output_model_config, 'w') as f:
            f.write(model_to_save.config.to_json_string())

    def load_model(self, path, config_file='config.json', model_file='model.wt'):
        """
        Load a previously saved model
        :param path: Directory where to find the model
        :param config_file: Name of the file where the configuration of the model is stored
        :param model_file: Name of the file where the model itself is stored
        :return: None
        """
        self.starting_model = self._load_model(path, self.model_cls, config_file, model_file, **self.model_params)
        self.model = copy.deepcopy(self.starting_model)

    def _split_data(self, data):
        """
        Splits the given dataset into self.cv folds with each fold maintaining the class ratio in the original dataset
        :param data: Dataset to split. Should be a list containing [input_ids, input_masks, segment_ids, labels]
        :return: Generator returning the splits one by one
        """
        np_data = [np.array(x) for x in data]
        # Count items in each class
        counted = Counter(np_data[-1])

        # Produce an array the same size of the dataset where each element is a number specifying to which split will
        # the corresponding example belong to
        # The splits preserve the class balance in the data
        splits = np.zeros(np_data[-1].size, dtype=np.int)

        # Iterate over each class
        for label, count in counted.items():
            # Split the items in this class into self.cv equal parts.
            # array_split_indices contains indices where to split the array of all the examples in this class
            # to produce self.cv same size chunks
            array_split_indices = np.array([int(x*count/self.cv) for x in range(1, self.cv)])

            # Get an array of all the indices of examples from the current class
            label_indices = np.where(np_data[-1] == label)

            # Split the label_indices according to the array_split_indices as described
            label_splits = np.split(label_indices[0], array_split_indices)

            # For each indice chunk from label_splits, assign the appropriate fold number
            for i, split in enumerate(label_splits):
                splits[split] = i

        # Collect all the examples from the current split and yield them
        for split in range(self.cv):
            split_indices = (splits == split)  # True only for elements in the current split
            train_data, test_data = [], []
            for item in np_data:
                train_data.append(torch.from_numpy(item[np.logical_not(split_indices)]))
                test_data.append(torch.from_numpy(item[split_indices]))
            yield train_data, test_data

    def train_cv(self, data, scoring='f1', return_error_rate=True, continue_training=False, train_batch_size=16,
                 gradient_accumulation_steps=4, num_train_epochs=4, learning_rate=5e-5, warmup_proportion=0.1):
        """
        Method for using k-fold cross validation to train multiple models and check their generalization ability. The
        result of this method is not a trained model, but it returns an average performance across the folds to
        facilitate hyperparameter optimization

        :param data: Data which to split and train on
        :param scoring: Scoring metric used to report performance at the end of training
        :param return_error_rate: Will return error rate instead of performance if True
        :param continue_training: Whether to continue training the model from the current state or restore it to the
        starting state
        :param train_batch_size: Batch size for training
        :param gradient_accumulation_steps: Number of steps to split a single batch into to save GPU memory
        :param num_train_epochs: Number of epochs to train for
        :param learning_rate: Learning rate
        :param warmup_proportion: Proportion of training to perform linear learning rate warmup for.
        e.g., 0.1 = 10%% of training.
        :return: Average performance across folds
        """
        scores = []
        for train_data, test_data in tqdm(self._split_data(data), total=self.cv):
            self.train(train_data, continue_training=continue_training, train_batch_size=train_batch_size,
                       gradient_accumulation_steps=gradient_accumulation_steps, num_train_epochs=num_train_epochs,
                       learning_rate=learning_rate, warmup_proportion=warmup_proportion)
            score = self.evaluate(test_data, scoring)
            scores.append(score)

        if return_error_rate:
            return 1 - np.mean(scores)
        else:
            return np.mean(scores)

    def train(self, data, continue_training=False, train_batch_size=16, gradient_accumulation_steps=4,
              num_train_epochs=4, learning_rate=5e-5, warmup_proportion=0.1):
        """
        Trains the model on given data

        :param data: Data to train on
        :param continue_training: Whether to continue training the model from the current state or restore it to the
        starting state
        :param train_batch_size: Batch size for training
        :param gradient_accumulation_steps: Number of steps to split a single batch into to save GPU memory
        :param num_train_epochs: Number of epochs to train for
        :param learning_rate: Learning rate
        :param warmup_proportion: Proportion of training to perform linear learning rate warmup for.
        e.g., 0.1 = 10%% of training.
        :return: None
        """
        if not continue_training:
            self.model = copy.deepcopy(self.starting_model)
            self.model.to(self.device)

        train_batch_size = train_batch_size // gradient_accumulation_steps

        num_train_optimization_steps = int(
            len(data[0]) / train_batch_size / gradient_accumulation_steps) * num_train_epochs
        if self.local_rank != -1:
            num_train_optimization_steps = num_train_optimization_steps // torch.distributed.get_world_size()

        if self.local_rank != -1:
            try:
                from apex.parallel import DistributedDataParallel as DDP
            except ImportError:
                raise ImportError(
                    "Please install apex from https://www.github.com/nvidia/apex to use distributed and fp16 training.")

            self.model = DDP(self.model)
        elif self.n_gpu > 1:
            self.model = torch.nn.DataParallel(self.model)

        # Prepare optimizer
        param_optimizer = list(self.model.named_parameters())

        # hack to remove pooler, which is not used
        # thus it produce None grad that break apex
        param_optimizer = [n for n in param_optimizer if 'pooler' not in n[0]]

        no_decay = ['bias', 'LayerNorm.bias', 'LayerNorm.weight']
        optimizer_grouped_parameters = [
            {'params': [p for n, p in param_optimizer if not any(nd in n for nd in no_decay)], 'weight_decay': 0.01},
            {'params': [p for n, p in param_optimizer if any(nd in n for nd in no_decay)], 'weight_decay': 0.0}
        ]

        optimizer = BertAdam(optimizer_grouped_parameters,
                             lr=learning_rate,
                             warmup=warmup_proportion,
                             t_total=num_train_optimization_steps)

        global_step = 0
        train_data = TensorDataset(*data)
        if self.local_rank == -1:
            train_sampler = RandomSampler(train_data)
        else:
            train_sampler = DistributedSampler(train_data)
        train_dataloader = DataLoader(train_data, sampler=train_sampler, batch_size=train_batch_size)

        self.model.train()
        for _ in trange(int(num_train_epochs), desc="Epoch"):
            tr_loss = 0
            nb_tr_examples, nb_tr_steps = 0, 0
            for step, batch in enumerate(tqdm(train_dataloader, desc="Iteration")):
                batch = tuple(t.to(self.device) for t in batch)
                input_ids, input_mask, segment_ids, label_ids = batch
                loss = self.model(input_ids, segment_ids, input_mask, label_ids)
                if self.n_gpu > 1:
                    loss = loss.mean()  # mean() to average on multi-gpu.
                if gradient_accumulation_steps > 1:
                    loss = loss / gradient_accumulation_steps
                tr_loss += loss.item()
                nb_tr_examples += input_ids.size(0)
                nb_tr_steps += 1

                loss.backward()
                if (step + 1) % gradient_accumulation_steps == 0:
                    optimizer.step()
                    optimizer.zero_grad()
                    global_step += 1

    def evaluate(self, data, scoring='f1', eval_batch_size=8):
        """
        Evaluate the model on given data

        :param data: Data to evaluate on
        :param scoring: Scoring method to report performance with. Can be a list of multiple metrics.
        :param eval_batch_size: Batch size of the evaluation loop
        :return: If scoring is not a list, returns the performance. If it is a list, returns a list of performances
        according to the metrics in the scoring list.
        """
        self.model.to(self.device)
        eval_data = TensorDataset(*data)
        eval_sampler = SequentialSampler(eval_data)
        eval_dataloader = DataLoader(eval_data, sampler=eval_sampler, batch_size=eval_batch_size)

        metrics = {
            'f1': mt.f1_score,
            'accuracy': mt.accuracy_score,
            'recall': mt.recall_score,
            'precision': mt.precision_score
        }
        if type(scoring) == str:
            results = ev.eval_model(self.model, eval_dataloader, [metrics[scoring]], self.device)
            return results[0]
        else:
            results = ev.eval_model(self.model, eval_dataloader, [metrics[s] for s in scoring], self.device)
            return results

    def predict(self, data, batch_size=8):
        """
        Predict on given data

        :param data: Data to predict on
        :param batch_size: Batch size in the predict loop
        :return: Tuple of 2 items. First is a list of predictions, the other is a list of corresponding labels
        """
        self.model.to(self.device)
        data = TensorDataset(*data)
        sampler = SequentialSampler(data)
        dataloader = DataLoader(data, sampler=sampler, batch_size=batch_size)

        return ev.predict_model(self.model, dataloader, self.device)

    def _init_from_finetuned(self, finetuned_path, finetuned_class, config_file='config.json', model_file='model.wt',
                            **kwargs):
        """
        Load a saved model and use its BERT weights

        :param finetuned_path: Directory where to find the model
        :param finetuned_class: Class of the model to load. Should be a class from the pytorch_pretrained_bert library
        :param config_file: Configuration file of the model
        :param model_file: File where the model is stored
        :param kwargs: Arguments given to the construcotr of finetuned_class
        :return: None
        """
        finetuned = self._load_model(finetuned_path, finetuned_class, config_file=config_file, model_file=model_file,
                                     **kwargs)
        self.model.bert = finetuned.bert

    def _load_model(self, path, model_cls, config_file='config.json', model_file='model.wt', **kwargs):
        """
        Load a model and return its instance

        :param path: Directory where to find the model
        :param model_cls: Class of the model
        :param config_file: Name of the configuration file
        :param model_file: Name of the weights of the model
        :param kwargs: Arguments given to the constructor of model_cls
        :return: The loaded model
        """
        config_path = os.path.join(path, config_file)
        model_path = os.path.join(path, model_file)
        config = BertConfig(config_path)
        model = model_cls(config, **kwargs)
        model.load_state_dict(torch.load(model_path))
        return model
