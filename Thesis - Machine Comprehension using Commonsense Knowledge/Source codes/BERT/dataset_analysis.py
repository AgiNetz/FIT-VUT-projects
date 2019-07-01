"""
File: dataset_analysis.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for analysis of various datasets
"""
import os
import sys
from collections import Counter
from matplotlib import pyplot as plt

import numpy as np

import datasets as dt
import utils as ut


def analyse_mcscript(data_dir, train_file='train-data.xml', dev_file='dev-data.xml', val_file='test-data.xml'):
    """
    Performs an analysis of the MCScript dataset.

    Reports the size of the dataset, ratio of individual sets, proportion of commonsense question,
    proportion of classes, performs an analysis of question type and plots a pie chart describing it.
    Similar statistics are reported for the test set individually.

    :param data_dir: Directory where to find the dataset
    :param train_file: File containing the training set of the dataset
    :param dev_file: File containing the development set of the dataset
    :param val_file: File containing the test set of the dataset
    :return:
    """
    # Load dataset
    train = dt.read_mcscript_examples(os.path.join(data_dir, train_file), is_training=True)
    dev = dt.read_mcscript_examples(os.path.join(data_dir, dev_file), is_training=True)
    val = dt.read_mcscript_examples(os.path.join(data_dir, val_file), is_training=True)

    # Calculate statistics for the whole dataset
    complete = train + dev + val
    complete_size = len(complete)

    print("Size: %d" % complete_size)

    print("Train portion: %f" % (len(train)/complete_size))
    print("Dev portion: %f" % (len(dev)/complete_size))
    print("Val portion: %f" % (len(val)/complete_size))

    types = np.array(ut.select_field_basic(complete, 'is_commonsense'))

    print("Commonsense proportion: %f" % (np.sum(types)/types.size))

    labels = ut.select_field_basic(complete, 'label')

    print("Class 1 proportion: %f" % (sum(labels)/len(labels)))

    question_types = ut.do_question_type_analysis(complete)

    question_type_count = Counter(question_types)

    sort_indices = np.argsort([x for x in question_type_count.keys()])
    plt.pie(np.array([x for x in question_type_count.values()])[sort_indices],
            labels=np.array([x for x in question_type_count.keys()])[sort_indices],  autopct="%.2f%%")
    plt.show()

    # Calculate statistics for the test set separately
    print("Validation size: %d" % len(val))

    val_types = np.array(ut.select_field_basic(val, 'is_commonsense'))

    print("Val commonsense proportion: %f", (np.sum(val_types)/val_types.size))

    val_labels = ut.select_field_basic(val, 'label')

    print("Val class 1 proportion: %f" % (sum(val_labels) / len(val_labels)))

    val_question_types = ut.do_question_type_analysis(val)
    val_question_type_count = Counter(val_question_types)

    sort_indices = np.argsort([x for x in val_question_type_count.keys()])
    plt.pie(np.array([x for x in val_question_type_count.values()])[sort_indices],
            labels=np.array([x for x in val_question_type_count.keys()])[sort_indices],  autopct="%.2f%%")
    plt.show()


if __name__ == "__main__":
    analyse_mcscript(sys.argv[1])
