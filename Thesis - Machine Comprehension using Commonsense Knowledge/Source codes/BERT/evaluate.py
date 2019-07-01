"""
File: evaluate.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for evaluating trained models
"""
import sys
import json
import os

import torch
from pytorch_pretrained_bert.modeling import BertConfig, BertForSequenceClassification, \
    BertForMultipleChoice, BertForNextSentencePrediction, BertForPreTraining

import bert_cv as bcv
import utils as ut


def evaluate(config):
    """
    Main method for evaluating models

    :param config: Configuration of the model and the evaluation to use
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


    # Load and transform data into features
    eval_examples = data_loader(os.path.join(eval_data_config["data_dir"], eval_data_config["val_file"]))
    eval_features = featurizer(eval_examples, model.tokenizer, model_config["max_seq_length"])

    all_input_ids = torch.tensor(field_selector(eval_features, 'input_ids'), dtype=torch.long)
    all_input_mask = torch.tensor(field_selector(eval_features, 'input_mask'), dtype=torch.long)
    all_segment_ids = torch.tensor(field_selector(eval_features, 'segment_ids'), dtype=torch.long)
    all_label = torch.tensor(ut.select_field_basic(eval_features, 'label'), dtype=torch.long)

    # Run the evaluation
    results = model.evaluate([all_input_ids, all_input_mask, all_segment_ids, all_label],
                             eval_config["metrics"], eval_config["eval_batch_size"])

    # Report and save results
    report = ''.join([str(m + ': ' + str(results[i]) + '\n') for i, m in enumerate(eval_config["metrics"])])
    print(report)

    with open(os.path.join(model_config["model_dir"], eval_config["results_file"]), mode="w") as fp:
        fp.write(report)


if __name__ == "__main__":
    fp = open(sys.argv[1], mode="r")
    config = json.load(fp)
    fp.close()
    evaluate(config)
