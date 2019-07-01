"""
File: train.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module for training models
"""
import os
import json
import sys
import logging

import torch
from pytorch_pretrained_bert.modeling import BertConfig, BertForSequenceClassification, \
    BertForMultipleChoice, BertForNextSentencePrediction, BertForPreTraining
from hyperopt import fmin, hp, tpe
import hyperopt

import bert_cv as bcv
import utils as ut


def parse_param_space_from_config(space):
    """
    Parse hyperopt parameter space from string
    :param space: String containing the hyperopt space description
    :return: The object representing the hyperopt space
    """
    return eval(space)


def optim_obj(model, data, scoring, effective_batch_size, params, logger):
    """
    Optimization objective function for hyperopt optimization

    :param model: Model to optimize hyperparameters on
    :param data: Data to train the model on
    :param scoring: Scoring method to use in k-fold cross-validation
    :param effective_batch_size: Number of batches to feed the GPU at once. Several steps are performed to reach
    configured batch_size, but this approach saves memory.
    :param params: Parameters to the train_cv function
    :param logger: Logger object
    :return: Error rate of the model reported with the scoring method provided
    """
    batch_size = params['train_batch_size']
    if batch_size == 0 or ((batch_size & (batch_size - 1)) != 0):
        return {'status': hyperopt.STATUS_FAIL}
    gradient_steps = batch_size//effective_batch_size
    logger.info('Running with params:\n' + str(params))
    return model.train_cv(data, scoring, return_error_rate=True, continue_training=False,
                          gradient_accumulation_steps=gradient_steps, **params)


def train(config):
    """
    Main method of the module for training the modules

    :param config: Configuration of the model to train
    :return: None
    """
    # Set up shortcut variables
    task = config["task"]
    data_config = config["data"]
    model_config = config["model"]
    data_loader = ut.known_tasks[task]["data_loader"]
    featurizer = ut.known_tasks[task]["featurizer"]
    field_selector = ut.known_tasks[task]["field_selector"]
    model_cls = ut.known_tasks[task]["model_cls"]
    model_params = ut.known_tasks[task]["model_params"]

    #Setup logging and output directory
    if os.path.exists(model_config["model_dir"]) and os.listdir(model_config["model_dir"]):
        raise ValueError("Output directory ({}) already exists and is not empty.".format(model_config["model_dir"]))
    os.makedirs(model_config["model_dir"], exist_ok=True)

    logging.basicConfig(format='%(levelname)s - %(name)s:  %(message)s',
                        filename=os.path.join(model_config["model_dir"], 'log.log'),
                        level=logging.INFO)
    logger = logging.getLogger(task)

    # Load training and development data
    train_examples = data_loader(os.path.join(data_config["data_dir"], data_config["train_file"]))
    if not data_config["no_dev"]:
        train_examples.extend(data_loader(os.path.join(data_config["data_dir"], data_config["dev_file"])))

    # Instantiate a model with provided configuration
    model = bcv.BERTwithCV(model_config["name"], model_cls, model_config["cv"],
                           model_config.get("load_from_finetuned", None), eval(model_config.get("finetuned_cls", "None")),
                           model_config.get("finetuned_config", 'config.json'),
                           model_config.get("finetuned_file", 'model.wt'), model_config.get("finetuned_params", {}),
                           model_params, model_config.get("local_rank", -1), model_config.get("no_cuda", False),
                           load_pretrained=model_config["use_pretrained"])

    # Transform data into features used by BERT
    train_features = featurizer(train_examples, model.tokenizer, model_config["max_seq_length"])

    all_input_ids = torch.tensor(field_selector(train_features, 'input_ids'), dtype=torch.long)
    all_input_mask = torch.tensor(field_selector(train_features, 'input_mask'), dtype=torch.long)
    all_segment_ids = torch.tensor(field_selector(train_features, 'segment_ids'), dtype=torch.long)
    all_label = torch.tensor(ut.select_field_basic(train_features, 'label'), dtype=torch.long)

    # Train the model with/without hyperparameter optimization
    if model_config["find_optimal_hyperparams"]:
        hyperparam_space = parse_param_space_from_config(model_config["hyperparam_space"])
        best_params = fmin(fn=lambda params: optim_obj(model, [all_input_ids, all_input_mask, all_segment_ids, all_label],
                                                       model_config["cv_scoring"], model_config["effective_batch_size"],
                                                       params, logger),
                           space=hyperparam_space,
                           algo=tpe.suggest,
                           max_evals=model_config["param_search_iters"])

        logger.info("Best params found:\n" + str(best_params))
    else:
        best_params = model_config["hyperparams"]

    model.train([all_input_ids, all_input_mask, all_segment_ids, all_label], continue_training=False,
                gradient_accumulation_steps=best_params["train_batch_size"]//model_config["effective_batch_size"],
                **best_params)

    # Save the model and other important information
    model.save_model(model_config["model_dir"], model_config.get("config_file", 'config.json'),
                     model_config.get('model_file', 'model.wt'))

    with open(os.path.join(model_config["model_dir"], "hyperparams.txt"), mode="w") as fp:
        fp.write(str(best_params))

    with open(os.path.join(model_config["model_dir"], 'training_config.json'), mode="w") as fp:
        json.dump(config, fp)


if __name__ == "__main__":
    fp = open(sys.argv[1], mode="r")
    config = json.load(fp)
    fp.close()
    train(config)
