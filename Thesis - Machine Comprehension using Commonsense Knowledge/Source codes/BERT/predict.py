"""
File: predict.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for using trained models for predicting on new data
"""
import sys
import json
import os
import csv

import torch

import bert_cv as bcv
import utils as ut


def predict(config, output_dir, output_file):
    """
    Main function of the module used to obtain the predictions and save them into a file

    :param config: Configuration of the model to load and use for prediction
    :param output_dir: Output directory where to save the predictions
    :param output_file: Name of the file with the results
    :return: None
    """
    # Setup shortcut variables
    task = config["task"]
    eval_config = config["evaluation"]
    eval_data_config = config["evaluation"]["data"]
    model_config = config["model"]
    data_loader = ut.known_tasks[task]["data_loader"]
    featurizer = ut.known_tasks[task]["featurizer"]
    field_selector = ut.known_tasks[task]["field_selector"]
    model_cls = ut.known_tasks[task]["model_cls"]
    model_params = ut.known_tasks[task]["model_params"]

    # Load the model
    model = bcv.BERTwithCV(model_config["name"], model_cls, model_config["cv"],
                           model_config.get("load_from_finetuned", None),
                           eval(model_config.get("finetuned_cls", "None")),
                           model_config.get("finetuned_config", 'config.json'),
                           model_config.get("finetuned_file", 'model.wt'), model_config.get("finetuned_params", {}),
                           model_params, model_config.get("local_rank", -1), model_config.get("no_cuda", False))

    model.load_model(model_config["model_dir"], model_config.get("config_file", 'config.json'),
                     model_config.get('model_file', 'model.wt'))

    # Load the dataset
    eval_examples = data_loader(os.path.join(eval_data_config["data_dir"], eval_data_config["val_file"]))
    eval_features = featurizer(eval_examples, model.tokenizer, model_config["max_seq_length"])

    all_input_ids = torch.tensor(field_selector(eval_features, 'input_ids'), dtype=torch.long)
    all_input_mask = torch.tensor(field_selector(eval_features, 'input_mask'), dtype=torch.long)
    all_segment_ids = torch.tensor(field_selector(eval_features, 'segment_ids'), dtype=torch.long)
    all_label = torch.tensor(ut.select_field_basic(eval_features, 'label'), dtype=torch.long)

    # Obtain the predictions
    predictions, labels = model.predict([all_input_ids, all_input_mask, all_segment_ids, all_label],
                                        eval_config["eval_batch_size"])

    # Save MCScript predictions
    if task in ('mcscript',  "mcscript_no_document", "mcscript_no_question", "mcscript_no_document_no_question"):
        question_types = ut.do_question_type_analysis(eval_examples)

        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        csv_labels = ("Document", "Scenario", "Question", "Question_type", "Is_commonsense", "Answer_1", "Answer_2", "Label", "Prediction")
        with open(os.path.join(output_dir, output_file), mode="w") as fp:
            writer = csv.writer(fp)
            writer.writerow(csv_labels)
            for i, example in enumerate(eval_examples):
                writer.writerow((example.document, example.scenario, example.question, question_types[i],
                                example.is_commonsense, example.answers[0], example.answers[1], labels[i], predictions[i]))
    else:
        # Other tasks currently not supported
        pass


if __name__ == "__main__":
    fp = open(sys.argv[1], mode="r")
    config = json.load(fp)
    fp.close()
    predict(config, sys.argv[2], sys.argv[3])
