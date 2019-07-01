"""
File: attentive_reader.py
Author: Tomáš Daniš
Login: xdanis05
Description: Module holding PyTorch neural modules.
"""

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.nn.utils.rnn as utrnn


class AttReader(nn.Module):
    """
    Module representing the modified stanford attentive reader architecture. This architecture is intended to be used
    with the MCScript dataset, but can be used with any data of the same input format.
    """
    def __init__(self, dict_size, embed_size=100, hidden_size=100, rnn_layers=1, dropout=0.2, bidir=True, *args,
                 **kwargs):
        """
        :param dict_size: Number of known words in the embedding layer
        :param embed_size: Size of the embeddings
        :param hidden_size: Size of the hidden state for the module's GRU
        :param rnn_layers: Number of layers of the module's GRU
        :param dropout: Dropout ratio used for regularization
        :param bidir: Whether the GRU is bidirectional
        :param args: Not used, present due to interface compability
        :param kwargs: Not used, present due to interface compability
        """
        super(AttReader, self).__init__()
        self.embedding = nn.Embedding(dict_size, embed_size, padding_idx=1)

        self.GRU = nn.GRU(input_size=embed_size, hidden_size=hidden_size, num_layers=rnn_layers, dropout=dropout,
                            bidirectional=bidir)
        self.attn = nn.Bilinear(2 * hidden_size if bidir else hidden_size,
                                2 * hidden_size if bidir else hidden_size,
                                1)
        self.a_to_qdoc_intersection = nn.Bilinear(2 * hidden_size if bidir else hidden_size,
                                                  2 * hidden_size if bidir else hidden_size,
                                                  1, bias=False)
        self.drop = nn.Dropout(dropout)

    def load_embeddings(self, embeddings):
        """
        Loads pretrained embeddings into the embedding layer of the model

        :param embeddings: torch tensor containing the embeddings to load
        :return: None
        """
        self.embedding.weight.data.copy_(embeddings)
        self.embedding.weight.requires_grad=False

    def embed_and_contextualize(self, sequence, lengths, rnn):
        """
        Helper method that transforms the padded input tokens into a contextualized vectors by transforming the input
        into word embeddings and running it through a supplied RNN unit.

        :param sequence: The input sequence to transform. Should be a tensor in the shape of (input_size, batch_size).
        Input size is the length all input sequences are padded to
        :param lengths: Original lengths of the input sequences without padding. Tensor in the shape of (batch_size)
        :param rnn: Recurrent neural network module which contextualizes the input
        :return: The output of the RNN module after running the input sequence through it
        """
        embed = self.embedding(sequence)
        # pack_padded_sequence requires the input to be sorted, we need to sort the sequences and the lengths the same
        lengths2, indices = torch.sort(lengths, descending=True)
        embed = torch.index_select(embed, dim=1, index=indices)
        # Input is padded, but we do not want the RNN to see the padding, use packed sequence.
        pack = torch.nn.utils.rnn.pack_padded_sequence(embed, lengths2)
        hidden, last_hidden = rnn(pack)
        # Re-pad the contextualized output for further processing
        hidden = torch.nn.utils.rnn.pad_packed_sequence(hidden)[0]
        # Restore the original input order
        _, indices = torch.sort(indices, descending=False)
        return torch.index_select(hidden, dim=1, index=indices), torch.index_select(last_hidden, dim=1, index=indices)

    def forward(self, x):
        """
        Implements a single forward pass through the model

        :param x: Input into the model. Should contain the following attributes:
            document - holding the document tokens
            question - question tokens
            answer1 - First answer candidate tokens
            answer2 - Second answer candidate tokens
        :return: Tensor of shape (2, batch_size) where each pairs contains scores for both answers for the question
        """
        doc_hidden = self.embed_and_contextualize(x.document[0], x.document[1], self.GRU)[0]
        q_hidden = self.embed_and_contextualize(x.question[0], x.question[1], self.GRU)[1]
        q_hidden = self.drop(q_hidden)
        q_hidden = torch.cat((q_hidden[0, :, :], q_hidden[1, :, :]), dim=-1)
        q_hidden = q_hidden.repeat(doc_hidden.size(0), 1, 1)

        a1_hidden = self.embed_and_contextualize(x.answer1[0], x.answer1[1], self.GRU)[1]
        a1_hidden = self.drop(a1_hidden)
        a1_hidden = torch.cat((a1_hidden[0, :, :], a1_hidden[1, :, :]), dim=-1)
        a2_hidden = self.embed_and_contextualize(x.answer2[0], x.answer2[1], self.GRU)[1]
        a2_hidden = self.drop(a2_hidden)

        a2_hidden = torch.cat((a2_hidden[0, :, :], a2_hidden[1, :, :]), dim=-1)
        attn = self.attn(doc_hidden, q_hidden).squeeze()
        weights = torch.transpose(F.softmax(attn, dim=0), 0, 1).unsqueeze(2)

        doc_hidden = torch.transpose(doc_hidden, 0, 1).transpose(1, 2)
        doc_reps = torch.bmm(doc_hidden, weights).squeeze()

        doc_reps = torch.stack((doc_reps, doc_reps), dim=1)
        answers = torch.stack((a1_hidden, a2_hidden), dim=1)
        answer_evals = self.a_to_qdoc_intersection(doc_reps, answers).squeeze()
        return answer_evals


class DummyModel(nn.Module):
    def __init__(self, dict_size, embed_size=100, hidden_size=100):
        super(DummyModel, self).__init__()
        self.embedding = nn.Embedding(dict_size, embed_size, padding_idx=1)

        self.fc1 = nn.Linear(hidden_size*2, hidden_size*2+50)
        self.fc2 = nn.Linear(hidden_size * 2+50, hidden_size)
        self.fc3 = nn.Linear(hidden_size, hidden_size)
        self.fc_a = nn.Linear(hidden_size,hidden_size)
        self.drop = nn.Dropout(p=0.4)

    def load_embeddings(self, embeddings):
        self.embedding.weight.data.copy_(embeddings)
        self.embedding.weight.requires_grad=False

    def forward(self, x):
        doc_hidden = self.embedding(x.document[0]).transpose(0,1).mean(1)
        q_hidden = self.embedding(x.question[0]).transpose(0,1).mean(1)
        a1_hidden = self.embedding(x.answer1[0]).transpose(0,1).mean(1)
        a2_hidden = self.embedding(x.answer2[0]).transpose(0,1).mean(1)

        q_d = torch.cat((doc_hidden,q_hidden),1)
        t = self.drop(F.tanh(self.fc2(self.drop(F.relu(self.fc1(q_d))))))

        answers = self.drop(self.fc_a(torch.stack((a1_hidden, a2_hidden), dim=1)))
        score_a1 = torch.bmm(answers[:,0,:].unsqueeze(1),t.unsqueeze(2))
        score_a2 = torch.bmm(answers[:,1,:].unsqueeze(1),t.unsqueeze(2))
        score = torch.stack((score_a1, score_a2),1).squeeze(-1).squeeze(-1)
        # answer_evals = torch.bmm(answers, doc_reps.unsqueeze(2)).squeeze()
        return score

class DummyModel2(nn.Module):
    def __init__(self, dict_size, embed_size=100, hidden_size=100,rnn_layers=1, dropout=0.2, bidir=True):
        super(DummyModel2, self).__init__()
        self.embedding = nn.Embedding(dict_size, embed_size, padding_idx=1)

        self.fc1 = nn.Linear(hidden_size * 4, hidden_size * 4 + 50)
        self.fc2 = nn.Linear(hidden_size * 4 + 50, hidden_size)
        self.fc3 = nn.Linear(hidden_size, hidden_size)
        self.fc_a = nn.Linear(hidden_size*2, hidden_size)
        self.drop = nn.Dropout(p=0.4)
        self.GRU = nn.GRU(input_size=embed_size, hidden_size=hidden_size, num_layers=rnn_layers, dropout=dropout,
                          bidirectional=bidir)

    def load_embeddings(self, embeddings):
        self.embedding.weight.data.copy_(embeddings)
        self.embedding.weight.requires_grad = False

    def embed_and_contextualize(self, sequence, lengths, rnn):
        embed = self.embedding(sequence)
        lengths2, indices = torch.sort(lengths, descending=True)
        embed = torch.index_select(embed, dim=1, index=indices)
        pack = torch.nn.utils.rnn.pack_padded_sequence(embed, lengths2)
        hidden, last_hidden = rnn(pack)
        hidden = torch.nn.utils.rnn.pad_packed_sequence(hidden)[0]
        _, indices = torch.sort(indices, descending=False)
        return torch.index_select(hidden, dim=1, index=indices), torch.index_select(last_hidden, dim=1, index=indices)

    def forward(self, x):
        #doc_hidden = self.embedding(x.document[0]).transpose(0, 1).mean(1)
        #q_hidden = self.embedding(x.question[0]).transpose(0, 1).mean(1)
        #a1_hidden = self.embedding(x.answer1[0]).transpose(0, 1).mean(1)
        #a2_hidden = self.embedding(x.answer2[0]).transpose(0, 1).mean(1)
        cathiddens = lambda h: torch.cat((h[-1, :, :], h[-2, :, :]), dim=-1)
        doc_hidden = self.embed_and_contextualize(x.document[0], x.document[1], self.GRU)[1]
        doc_hidden = cathiddens(doc_hidden)
        q_hidden = self.embed_and_contextualize(x.question[0], x.question[1], self.GRU)[1]
        q_hidden = cathiddens(q_hidden)

        a1_hidden = self.embed_and_contextualize(x.answer1[0], x.answer1[1], self.GRU)[1]
        a1_hidden = cathiddens(a1_hidden)
        a2_hidden = self.embed_and_contextualize(x.answer2[0], x.answer2[1], self.GRU)[1]
        a2_hidden = cathiddens(a2_hidden)

        q_d = torch.cat((doc_hidden, q_hidden), 1)
        t = self.drop(F.tanh(self.fc2(self.drop(F.relu(self.fc1(q_d))))))

        answers = self.drop(self.fc_a(torch.stack((a1_hidden, a2_hidden), dim=1)))
        score_a1 = torch.bmm(answers[:, 0, :].unsqueeze(1), t.unsqueeze(2))
        score_a2 = torch.bmm(answers[:, 1, :].unsqueeze(1), t.unsqueeze(2))
        score = torch.stack((score_a1, score_a2), 1).squeeze(-1).squeeze(-1)
        # answer_evals = torch.bmm(answers, doc_reps.unsqueeze(2)).squeeze()
        return score

