"""
File: utils.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module containing various utility functions useful in different parts of the projects
"""
import os
import torch
import numpy as np

from pytorch_pretrained_bert.modeling import BertConfig, BertForSequenceClassification, \
    BertForMultipleChoice, BertForNextSentencePrediction, BertForPreTraining
import datasets as dt
import featurizers as ft


def select_field_basic(features, field):
    """
    From a list of examples in a dataset, selects a specific attribute from all the examples.
    :param features: List of examples in a dataset
    :param field: Attribute to choose
    :return: List of the field attribute from all the examples in features
    """
    return [getattr(feature, field) for feature in features]


def select_field_multiplechoice(features, field):
    """
    From a list of multiple-choice examples in a dataset, selects a specific attribute from all answer candidates
    :param features: List of examples in a dataset
    :param field: Attribute to choose
    :return: List of the field attribute from all the examples in features
    """
    return [
        [
            choice[field]
            for choice in feature.choices_features
        ]
        for feature in features
    ]


# Known tasks train.py and evaluate.py can do and their configurations
# This dictionary should be added to if you want to use train.py for your own scripts
known_tasks = {
    "mcscript":
    {
        "data_loader": dt.read_mcscript_examples,
        "featurizer": ft.convert_MCScript_examples_to_features,
        "field_selector": select_field_multiplechoice,
        "model_cls": BertForMultipleChoice,
        "model_params": {"num_choices": 2}
    },
    "mcscript_no_document":
    {
        "data_loader": dt.read_mcscript_examples,
        "featurizer": lambda x, y, z: ft.convert_MCScript_examples_to_features(x,y,z, no_document=True),
        "field_selector": select_field_multiplechoice,
        "model_cls": BertForMultipleChoice,
        "model_params": {"num_choices": 2}
    },
    "mcscript_no_question":
    {
        "data_loader": dt.read_mcscript_examples,
        "featurizer": lambda x, y, z: ft.convert_MCScript_examples_to_features(x, y, z, no_question=True),
        "field_selector": select_field_multiplechoice,
        "model_cls": BertForMultipleChoice,
        "model_params": {"num_choices": 2}
    },
    "mcscript_no_document_no_question":
    {
        "data_loader": dt.read_mcscript_examples,
        "featurizer": lambda x, y, z: ft.convert_MCScript_examples_to_features(x, y, z,
                                                                               no_question=True, no_document=True),
        "field_selector": select_field_multiplechoice,
        "model_cls": BertForMultipleChoice,
        "model_params": {"num_choices": 2}
    },
    "swag":
    {
        "data_loader": dt.read_swag_examples,
        "featurizer": ft.convert_swag_examples_to_multiple_choice_features,
        "field_selector": select_field_multiplechoice,
        "model_cls": BertForMultipleChoice,
        "model_params": {"num_choices": 4}
    }
}



def split_to_binary_confusion_matrix(predictions, labels):
    """
    Constructs a confusion matrix for binary classification problems and returns all its elements
    :param predictions: Predictions
    :param labels: Ground truth
    :return: Tuple of 4 items (True positives, True negatives, False positives, False negatives)
    """
    labels = np.array(labels)
    predictions = np.array(predictions)

    pos_preds = np.argwhere(predictions == 1)
    neg_preds = np.argwhere(predictions == 0)
    pos_ann = np.argwhere(labels == 1)
    neg_ann = np.argwhere(labels == 0)

    tp = np.intersect1d(pos_ann, pos_preds)
    tn = np.intersect1d(neg_ann, neg_preds)
    fp = np.intersect1d(neg_ann, pos_preds)
    fn = np.intersect1d(pos_ann, neg_preds)

    return tp, tn, fp, fn


def do_question_type_analysis(examples):
    """
    Performs question type analysis on the MCScript dataset.
    The analysis splits the questions into the following classes:
    what/which, who/whose, why, where, when, how, how long/often, how many/much, yes/no, other
    :param examples: List of examples from the MCScript dataset
    :return: List of the same size as examples. Each element is the question type of the
    corresponding example from the dataset
    """
    question_types = []
    for example in examples:
        question = example.question.lower()
        q_words = question.split(sep=' ')
        answer_words = example.answers[0].lower().split()

        if q_words[0] == 'what' or q_words[0] == 'which':
            question_types.append('what/which')
        elif q_words[0] == 'who' or q_words[0] == 'whose':
            question_types.append('who/whose')
        elif q_words[0] in ['why', 'where', 'when']:
            question_types.append(q_words[0])
        elif q_words[0] == 'how':
            if q_words[1] == 'long' or q_words[1] == 'often':
                question_types.append('how long/often')
            elif q_words[1] == 'many' or q_words[1] == 'much':
                question_types.append('how many/much')
            else:
                question_types.append('how')
        elif answer_words[0] == 'yes' or answer_words[0] == 'no':
            question_types.append('yes/no')
        else:
            question_types.append('other')

    return question_types
