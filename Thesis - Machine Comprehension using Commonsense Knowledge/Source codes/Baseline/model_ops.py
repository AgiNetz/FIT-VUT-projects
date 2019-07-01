"""
File: model_ops.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module holding functions implementing model operations - such as training, predicting or evaluating
"""

import torch.nn as nn
import torch.nn.utils as tut
import torch.optim as optim
import torch
from tqdm import tqdm

import torchtext.data as torch_data

import conf
from baseline import totext

_DEBUG = False # Debugging flag


def train(net, train_iterator, dev_iterator, vocabs, epoch_num=4, lr=0.002):
    """
    Train a given model on the given dataset

    :param net: Model to train
    :param train_iterator: Iterator through a training set of the dataset to use
    :param dev_iterator: Iterator through a development set of the dataset to use
    :param vocabs: Vocabularies used in the dataset, only used for debugging
    :param epoch_num: Number of epochs to train for
    :param lr: Learning rate to train with
    :return: None
    """
    net.train()

    criterion = nn.CrossEntropyLoss()

    parameters = filter(lambda p: p.requires_grad, net.parameters())
    optimizer = optim.Adamax(parameters, lr=lr)

    # Training loop
    for epoch in tqdm(range(epoch_num), total=epoch_num, desc="Epoch"):
        epoch_loss = 0
        # Epoch loop
        for i, batch in tqdm(enumerate(train_iterator), total=len(train_iterator), desc="Iteration"):
            net.train()
            if _DEBUG:
                q = totext(batch.question[0],vocabs[0],batch_first=False)
                d = totext(batch.document[0],vocabs[0],batch_first=False)
                a1 = totext(batch.answer1[0],vocabs[0],batch_first=False)
                a2 = totext(batch.answer2[0],vocabs[0],batch_first=False)
                print("* "*20+"NEXT"+"* "*20)
                print(d[0])
                print("* " * 20 + "Question" + "* " * 20)
                print(q[0])
                print("* " * 20 + "Answers" + "* " * 20)
                print(a1[0])
                print(a2[0])

            optimizer.zero_grad()
            out = net(batch)
            loss = criterion(out, batch.correct)
            loss.backward()
            tut.clip_grad_norm_(parameters, 0.5)
            optimizer.step()
            epoch_loss += loss.item()

        # At the end of an epoch, evaluate the current performance on the development set
        with torch.no_grad():
            net.eval()
            dev_loss = 0
            j = 0
            correct = 0
            total = 0
            for j, val_batch in enumerate(dev_iterator):
                out = net(val_batch)
                total += val_batch.correct.size(0)
                loss = criterion(out, val_batch.correct)
                dev_loss += loss.item()
                _, pred_indexes = torch.max(out.data, 1)
                correct += (pred_indexes == val_batch.correct).sum().item()
            print('Epoch: {0}, Train loss: {1}, Dev loss: {2}, Dev accuracy: {3}%'.format(
                epoch, epoch_loss/len(train_iterator), dev_loss/(j+1), correct*100/total))


def predict(net, input, fields):
    """
    Predict answers for the given input
    :param net: Model to predict with
    :param input: Input to predict on
    :param fields: Structure of the data the model expects
    :return: Predictions for the given inputs
    """
    net.eval()
    example = torch_data.Example.fromlist(input, fields)
    dataset = torch_data.Dataset([example])
    iterator = torch_data.Iterator(dataset, batch_size=1)
    net_in = next(iter(iterator))
    return predict_batch(net, net_in)


def predict_batch(net, batch):
    """
    Predicts a single batch using the model provided

    :param net: Model to predict with
    :param batch: Batch to predict on
    :return: Predictions
    """
    with torch.no_grad():
        out = net(batch)
        _, predicted = torch.max(out.data, 1)
        return predicted


def eval_model(net, val_iter):
    """
    Evaluate a model's performance on the given test set

    :param net: Model to evaluate
    :param val_iter: Data to evaluate on
    :return: A tuple with the first item being the accuracy. The second item is a list of F1 scores for all classes
    in the task.
    """
    correct = 0
    total = 0
    cm = conf.ConfusionMatrix([0, 1])
    net.eval()
    with torch.no_grad():
        for batch in val_iter:
            total += batch.correct.size(0)
            prediction = predict_batch(net, batch)
            cm.add_entry(batch.correct.tolist(), prediction.tolist())
            correct += (prediction == batch.correct).sum().item()

    return correct/total, cm.get_f1()

