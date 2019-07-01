"""
File: model_quality.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for qualitative analysis of models for various datasets
"""
import sys
import csv

import numpy as np
from collections import Counter
from matplotlib import pyplot as plt


def evaluate_mcscript_model_quality(file):
    """
    Reports qualitative indicators of
    :param file: CSV file with the data and corresponding predictions. Such a file can be produced by predict.py
    :return: None
    """
    # Read the CSV file
    with open(file, mode="r") as fp:
        reader = csv.reader(fp)
        next(reader, None)
        document, scenario, question, question_type, commonsense, \
        answer_1, answer_2,  label, prediction = (np.array(a) for a in list(zip(*[x for x in reader])))

    # Calculate and report qualitative indicators
    incorrect_answers = (label != prediction)
    commonsense = (commonsense == 'True')
    incorrect_commonsense_att = commonsense[incorrect_answers]
    print("Incorrect answers commonsense proportion: %f" % (sum(incorrect_commonsense_att)/incorrect_commonsense_att.size))
    print("Incorrectly answered commonsense questions proportion: %f" % (sum(incorrect_commonsense_att)/sum(commonsense)))
    incorrect_qtypes = question_type[incorrect_answers]

    incorrect_qtype_counts = Counter(incorrect_qtypes)
    sort_indices = np.argsort([x for x in incorrect_qtype_counts.keys()])
    plt.pie(np.array([x for x in incorrect_qtype_counts.values()])[sort_indices],
            labels=np.array([x for x in incorrect_qtype_counts.keys()])[sort_indices],  autopct="%.2f%%")
    plt.show()


if __name__ == "__main__":
    evaluate_mcscript_model_quality(sys.argv[1])
