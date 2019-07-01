"""
File: mcscript_reader.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for loading the dataset into an internal format used by the rest of the project
"""
import xml.etree.ElementTree as ET
import torchtext.data as torch_data


def read_mcscript(path, fields, train_suffix=r'/train-data.xml', dev_suffix=r'/dev-data.xml',
                  val_suffix=r'/test-data.xml',  num_train_examples=-1, num_dev_examples=-1, num_val_examples=-1):
    """
    Reads the MCScript dataset from the given directory, parses the XML and transforms it into a torchtext.Dataset class
    The function expects 3 XML files, one for each split of the dataset and the XMLs are expected to be in the format as
    published by the original authors of the dataset.

    :param path: Directory where to find the dataset
    :param fields: Torchtext fields defining the internal structure to which to transform the loaded data
    :param train_suffix: Train set file of the data, None to not load
    :param dev_suffix: Development set file of the data, None to not laod
    :param val_suffix: Test set file of the data, None to not load
    :param num_train_examples: Number of examples from the training set to load, -1 for all
    :param num_dev_examples: Number of examples from the development set to load, -1 for all
    :param num_val_examples: number of examples from the test set to load, -1 for all
    :return: A 3-item tuple corresponding to the training, development and test set, each either a torchtext.Dataset
    class if the respective set was loaded or None
    """
    datasets = []
    suffixes = [train_suffix, dev_suffix, val_suffix]
    nums = [num_train_examples, num_dev_examples, num_val_examples]
    # Iterate through the file to load and transform
    for set in range(3):
        # If a suffix is None, this set is not to be loaded
        if not suffixes[set]:
            datasets.append(None)
            continue
        examples = []
        # Parse the XML
        data = ET.parse(path + suffixes[set]).getroot()
        try:
            for instance in data:
                it = iter(instance)
                text = next(it) # Extract the document text
                questions = next(it) # Extrac the question text
                for question in questions:
                    correct = 0
                    example = [text.text, question.attrib['text']]
                    for i, answer in enumerate(question):
                        example.append(answer.attrib['text'])  # Extract answer texts
                        if answer.attrib['correct'] == 'True':  # Extract the label
                            correct = i
                    example.append(correct)
                    examples.append(torch_data.Example.fromlist(example, fields))  # Form an example
                    nums[set] -= 1  # Count how many examples were loaded
                    if nums[set] == 0:
                        datasets.append(torch_data.Dataset(examples, fields))
                        raise AssertionError
            datasets.append(torch_data.Dataset(examples, fields))  # Construct the dataset
        except AssertionError:
            continue

    return tuple(datasets)


if __name__ == "__main__":
    text_field = torch_data.Field(sequential=True, use_vocab=True, tokenize="spacy")
    label_field = torch_data.Field(sequential=False, use_vocab=False)
    example_template = [('document', text_field),
                        ('question', text_field),
                        ('answer1', text_field),
                        ('answer2', text_field),
                        ('correct', label_field)]
    sets = read_mcscript('MCScript', example_template, num_train_examples=2, num_dev_examples=2, num_val_examples=2)
    pass
