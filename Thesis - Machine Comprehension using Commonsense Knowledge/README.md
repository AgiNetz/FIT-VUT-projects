# Bachelor's thesis - Machine Comprehension using Commonsense Knowledge
This file serves as a usage manual for the "Machine Comprehension Using Commonsense Knowledge" thesis. It describes its structure and provides guidance to replication of the individual experiments described in the thesis.


## Project structure:
- xdanis05_thesis_source - Contains TeX source codes for the thesis report PDF.
- Source codes - Contains python source codes
- Source codes/Baseline - contains source codes for the stanford attentive reader baseline.
- Source codes/BERT - contains source codes and configurations for experiments with the BERT model.



## Usage:
### Enviroment setup
Source code directories for both models contain a requirements.txt file that can be used to set-up a python virtual enviroment with all the dependencies the projects need. It is recommended to use separate virtual enviroments the two models. The models have been tested with python version 3.6 and 3.7. To create a separate virtual enviroment and download all the dependencies required for the model, download virtualenv package using the following command.
```
pip install virtualenv
```

Afterwards, create a new virtual enviroment and download the dependencies using the following commands.
```
virtualenv venv
source venv/bin/activate
pip install -r requirements.txt
```

For the baseline, it is also necessary to download a spacy package language model using the command.

```python -m spacy download en```

### Working with the stanford attentive reader baseline
In the Baseline directory, a configuration file config.json is provided which has been used to train the model and obtain the results reported in the thesis. To replicate this training procedure, it is first necessary to download the MCScript dataset from http://www.sfb1102.uni-saarland.de/?page_id=2582. To use the default config file, name the directory with the dataset splits "MCScript" and place it into the Baseline directory. After that, activate the previously created virtual enviroment with the necessary dependencies and run the following command.\
\
```python baseline.py config.json train```\
\
The training process will begin and its progress can be monitored through a progress bar present. Once training has finished, evaluation on the test set will be displayed and the model will be saved into a "baseline" directory.To evaluate this saved model, use the command
\
```python baseline.py config.json eval```\
\
The model will be evaluated once again on test set of the MCScript dataset and results displayed.

### Working with the BERT model
To replicate any of the experiments described in the thesis, the MCScript dataset must first be downloaded from http://www.sfb1102.uni-saarland.de/?page_id=2582. To use the default config files, name the directory with the dataset splits "MCScript" and place it into the BERT directory. To replicate experiment number 2, the SWAG dataset must be downloaded as well from https://github.com/rowanz/swagaf/tree/master/data. From the provided repository, download files train.csv and val.csv and to use the default configuration files, place them into a directory named SWAG in the BERT directory. The BERT directory contains configurations files *experiment{1-5}_config.json* corresponding to the individual experiments.\
To train models for experiment 1 and 3-5, use the following following command.\
\
```python train.py experimentX_config.json```\
\
with the appropriate config file. With experiment 2, swag pre-trained model must first be trained using the command.\
\
```python train.py swag_config.json```\
\
After the training has completed, a model on MCScript can be trained using the following command.\
\
```python train.py experiment2_config.json```\
\
To evaluate models from any of the experiments, use the following command\
\
```python evaluate.py experimentX_config.json```\
\
To obtain predictions on the test set of the MCScript dataset to use in experiment analysis, use the following command. \
\
```python predict.py experimentX_config.json your_directory your_file.csv```\
\
This command generates a your_directory/your_file.csv CSV file with the data and the model predictions. These can be analyzed with the model_quality.py script the following way.\
\
```python model_quality.py your_directory/your_file.csv```\
\
Note that this scripts displays a pie chart figure, so it is recommended to run it on a machine with GUI.
