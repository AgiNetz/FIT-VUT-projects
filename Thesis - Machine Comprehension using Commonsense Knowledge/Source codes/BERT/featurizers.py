"""
File: featurizers.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for transforming data into tokens used by BERT as input
"""


# Helpers
def _truncate_seq_pair(tokens_a, tokens_b, max_length):
    """Truncates a sequence pair in place to the maximum length.
    Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py"""

    # This is a simple heuristic which will always truncate the longer sequence
    # one token at a time. This makes more sense than truncating an equal percent
    # of tokens from each, since if one sequence is very short then each token
    # that's truncated likely contains more information than a longer sequence.
    while True:
        total_length = len(tokens_a) + len(tokens_b)
        if total_length <= max_length:
            break
        if len(tokens_a) > len(tokens_b):
            tokens_a.pop()
        else:
            tokens_b.pop()


class MultipleChoiceInputFeatures(object):
    """
    Class representing multiple-choice question.
    Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py
    """
    def __init__(self,
                 example_id,
                 choices_features,
                 label

    ):
        self.example_id = example_id
        self.choices_features = [
            {
                'input_ids': input_ids,
                'input_mask': input_mask,
                'segment_ids': segment_ids
            }
            for _, input_ids, input_mask, segment_ids in choices_features
        ]
        self.label = label


def convert_MCScript_examples_to_features(examples, tokenizer, max_seq_length, no_question=False, no_document=False):
    """
    Converts a list of MCScriptExample-s from datasets.py into a list of MultipleChoiceInputFeature-s to use as BERT
    input. Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py

    :param examples: Examples to transform
    :param tokenizer: Tokenizer to use for transformation
    :param max_seq_length: Maximum sequence length of the input
    :param no_question: Whether to not include the question in the input
    :param no_document: Whether to not include the information document in the input
    :return: List of MultipleChoiceInputFeature-s to use as BERT input.
    """
    features = []
    for example_index, example in enumerate(examples):
        document_tokens = tokenizer.tokenize(example.document)
        question_tokens = tokenizer.tokenize(example.question)

        choices_features = []
        for answer_index, answer in enumerate(example.answers):
            # We create a copy of the context tokens in order to be
            # able to shrink it according to ending_tokens
            if no_document:
                document_tokens_choice = []
            else:
                document_tokens_choice = document_tokens[:]

            if no_question:
                question_tokens_choice = []
            else:
                question_tokens_choice = question_tokens[:]

            answer_tokens = tokenizer.tokenize(answer)
            # Modifies `context_tokens_choice` and `ending_tokens` in
            # place so that the total length is less than the
            # specified length.  Account for [CLS], [SEP], [SEP] with
            # "- 3"

            # We only want to concatenate the document - that is always longer than question + answer
            _truncate_seq_pair(document_tokens_choice, question_tokens_choice + answer_tokens, max_seq_length - 3)

            tokens = ["[CLS]"] + document_tokens_choice + question_tokens_choice + ["[SEP]"] + answer_tokens + ["[SEP]"]
            segment_ids = [0] * (len(document_tokens_choice + question_tokens_choice) + 2) + [1] * (len(answer_tokens) + 1)

            input_ids = tokenizer.convert_tokens_to_ids(tokens)
            input_mask = [1] * len(input_ids)

            # Zero-pad up to the sequence length.
            padding = [0] * (max_seq_length - len(input_ids))
            input_ids += padding
            input_mask += padding
            segment_ids += padding

            assert len(input_ids) == max_seq_length
            assert len(input_mask) == max_seq_length
            assert len(segment_ids) == max_seq_length

            choices_features.append((tokens, input_ids, input_mask, segment_ids))

        label = example.label
        features.append(
            MultipleChoiceInputFeatures(
                example_id=None,
                choices_features=choices_features,
                label=label
            )
        )

    return features


def convert_swag_examples_to_multiple_choice_features(examples, tokenizer, max_seq_length):
    """
    Converts a list of SwagExample-s from datasets.py into a list of MultipleChoiceInputFeature-s to use as BERT
    input. Credit to
    https://github.com/huggingface/pytorch-pretrained-BERT/blob/master/examples/run_swag.py

    :param examples: Examples to transform
    :param tokenizer: Tokenizer to use for transformation
    :param max_seq_length: Maximum sequence length of the input
    :param no_question: Whether to not include the question in the input
    :param no_document: Whether to not include the information document in the input
    :return: List of MultipleChoiceInputFeature-s to use as BERT input.
    """
    features = []
    for example_index, example in enumerate(examples):
        context_tokens = tokenizer.tokenize(example.context_sentence)
        start_ending_tokens = tokenizer.tokenize(example.start_ending)

        choices_features = []
        for ending_index, ending in enumerate(example.endings):
            # We create a copy of the context tokens in order to be
            # able to shrink it according to ending_tokens
            context_tokens_choice = context_tokens[:]
            ending_tokens = start_ending_tokens + tokenizer.tokenize(ending)
            # Modifies `context_tokens_choice` and `ending_tokens` in
            # place so that the total length is less than the
            # specified length.  Account for [CLS], [SEP], [SEP] with
            # "- 3"
            _truncate_seq_pair(context_tokens_choice, ending_tokens, max_seq_length - 3)

            tokens = ["[CLS]"] + context_tokens_choice + ["[SEP]"] + ending_tokens + ["[SEP]"]
            segment_ids = [0] * (len(context_tokens_choice) + 2) + [1] * (len(ending_tokens) + 1)

            input_ids = tokenizer.convert_tokens_to_ids(tokens)
            input_mask = [1] * len(input_ids)

            # Zero-pad up to the sequence length.
            padding = [0] * (max_seq_length - len(input_ids))
            input_ids += padding
            input_mask += padding
            segment_ids += padding

            assert len(input_ids) == max_seq_length
            assert len(input_mask) == max_seq_length
            assert len(segment_ids) == max_seq_length

            choices_features.append((tokens, input_ids, input_mask, segment_ids))

        label = example.label
        features.append(
            MultipleChoiceInputFeatures(
                example_id=example.swag_id,
                choices_features=choices_features,
                label=label
            )
        )

    return features
