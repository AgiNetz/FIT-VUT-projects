"""
File: conf.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module implementing a confusion matrix
"""
import numpy as np


class ConfusionMatrix:
    """
    Represents a confusion matrix used in classification evaluation
    """

    def __init__(self, labels):
        """
        Constructs an empty confusion matrix
        :param labels: Class labels used in the classification problem
        """
        self.class_to_index = dict.fromkeys(labels)
        for i, label in enumerate(labels):
            self.class_to_index[label] = i
        self.matrix = np.zeros((len(labels), len(labels)))

    def add_entry(self, truth, predicted):
        """
        Add an entry into the confusion matrix

        :param truth: Ground truth label
        :param predicted: Predicted label
        :return: None
        """
        for y, p in zip(truth, predicted):
            self.matrix[self.class_to_index[y], self.class_to_index[p]] += 1

    def get_f1(self):
        """
        Calculates F1-score for all classes and returns them in a list
        :return: F1 scores for all classes
        """
        scores = []
        for c in range(self.matrix.shape[0]):
            recall = self.matrix[c, c]/self.matrix[c].sum()
            precision = self.matrix[c, c] / self.matrix[::, c].sum()
            scores.append(2*recall*precision/(recall+precision))
        return scores
