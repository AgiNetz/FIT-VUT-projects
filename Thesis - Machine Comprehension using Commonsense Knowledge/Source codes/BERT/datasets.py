"""
File: datasets.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module containing custom for loading datasets and returning them in a format used by the project
"""
import xml.etree.ElementTree as ET
import csv


# ------------------------ MCScript ------------------------
class MCScriptExample():
    """
    Class representing a single MCScript example
    """
    def __init__(self,
                 document,
                 scenario,
                 question,
                 commonsense,
                 answer_0,
                 answer_1,
                 label=None):
        self.document = document
        self.question = question
        self.scenario = scenario
        self.is_commonsense = commonsense
        self.answers = [answer_0, answer_1]
        self.label = label

    def __repr__(self):
        return "{0}\n\n{1}\na, {2}\nb, {3}\nCorrect: {4}".format(self.document, self.question,
                                                                 self.answers[0], self.answers[1],
                                                                 self.label)

    def __str__(self):
        return self.__repr__()


def read_mcscript_examples(input_file, is_training=True):
    """
    Load MCScript examples from the given file and returns a list of MCScriptExample-s
    :param input_file: File with the data
    :param is_training: Not used, present for API compability
    :return: List of MCScriptExample-s
    """
    examples = []
    data = ET.parse(input_file).getroot()
    for instance in data:
        scenario = instance.attrib['scenario']
        text = instance[0]
        questions = instance[1]
        for question in questions:
            correct = 0
            example = [text.text, question.attrib['text']]
            is_commonsense = question.attrib['type'] == 'commonsense'
            for i, answer in enumerate(question):
                example.append(answer.attrib['text'])
                if answer.attrib['correct'] == 'True':
                    correct = i
            example.append(correct)
            examples.append(
                MCScriptExample(
                    document=example[0],
                    question=example[1],
                    scenario=scenario,
                    commonsense=is_commonsense,
                    answer_0=example[2],
                    answer_1=example[3],
                    label=example[4]
                )
            )

    return examples


# ------------------------ SWAG ------------------------
class SwagExample(object):
    """A single example for the SWAG dataset.
    Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py"""
    def __init__(self,
                 swag_id,
                 context_sentence,
                 start_ending,
                 ending_0,
                 ending_1,
                 ending_2,
                 ending_3,
                 label=None):
        self.swag_id = swag_id
        self.context_sentence = context_sentence
        self.start_ending = start_ending
        self.endings = [
            ending_0,
            ending_1,
            ending_2,
            ending_3,
        ]
        self.label = label

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        l = [
            "swag_id: {}".format(self.swag_id),
            "context_sentence: {}".format(self.context_sentence),
            "start_ending: {}".format(self.start_ending),
            "ending_0: {}".format(self.endings[0]),
            "ending_1: {}".format(self.endings[1]),
            "ending_2: {}".format(self.endings[2]),
            "ending_3: {}".format(self.endings[3]),
        ]

        if self.label is not None:
            l.append("label: {}".format(self.label))

        return ", ".join(l)


def read_swag_examples(input_file, is_training=True):
    """
    Load SWAG examples from the given file and returns a list of SwagExample-s
    Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py

    :param input_file: File with the data
    :param is_training: If false, labels are not expected
    :return: List of SwagExample-s
    """
    with open(input_file, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        lines = []
        for line in reader:
            lines.append(line)

    if is_training and lines[0][-1] != 'label':
        raise ValueError(
            "For training, the input file must contain a label column."
        )

    examples = [
        SwagExample(
            swag_id = line[2],
            context_sentence=line[4],
            start_ending=line[5],  # in the swag dataset, the common beginning of each choice is stored in "sent2".
            ending_0=line[7],
            ending_1=line[8],
            ending_2=line[9],
            ending_3=line[10],
            label=int(line[11]) if is_training else None
        ) for line in lines[1:]  # we skip the line with the column names
    ]

    return examples
