"""
File: evaluation.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module containing methods for evaluation of models
"""
import torch
import numpy as np


def eval_model(model, data_iter, metrics, device, **kwargs):
    """
    Evaluate the performance of a given model on the given data according to the given metrics

    :param model: Model to evaluate
    :param data_iter: Iterator for the data to evaluate on
    :param metrics: Metrics to use for reporting performance
    :param device: PyTorch device the model resides on
    :param kwargs: Arguments passed to the metric functions
    :return: List of same length as metrics where each element is the performance of the
    model according to the given metric
    """
    model.eval()
    predictions, labels = predict_model(model, data_iter, device)
    return [m(labels, predictions, **kwargs) for m in metrics]


def predict_model(model, data_iter, device):
    """
    Run a model on the given data and obtain predictions

    :param model: Model to evaluate
    :param data_iter: Iterator for the data to evaluate on
    :param device: PyTorch device the model resides on
    :return: Tuple of 2 items. First is a list of predictions, the other a list of labels
    """
    model.eval()
    predictions = []
    labels = []
    for input_ids, input_mask, segment_ids, label_ids in data_iter:
        input_ids = input_ids.to(device)
        input_mask = input_mask.to(device)
        segment_ids = segment_ids.to(device)
        label_ids = label_ids.to(device)

        with torch.no_grad():
            logits = model(input_ids, segment_ids, input_mask)

        predictions.extend(np.argmax(logits.detach().cpu().numpy(), axis=1))
        labels.extend(label_ids.to('cpu').numpy().tolist())

    return predictions, labels
