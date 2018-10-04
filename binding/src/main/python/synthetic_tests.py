import random
import time
import re
import os
import argparse
import numpy as np
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.pipeline import Pipeline, FeatureUnion
from sklearn.linear_model import SGDClassifier
from vgram import VGramBuilder, Tokenizer


def restore_test():
    alpha = [i for i in range(26)]
    vgb = VGramBuilder(len(alpha) + 100, alpha)
    for i in range(100000):
        l = random.randint(1, 300)
        seq = [alpha[random.randint(0, len(alpha) - 1)] for _ in range(l)]
        vgb.accept(seq)
    print(str(len(vgb.alphabet())) + ' < 31')
    print(vgb.parse([0, 5, 2, 6, 9, 98, 1]))


def download_data(cats=None):
    if cats is None:
        return fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
    return fetch_20newsgroups(subset='train', categories=cats), fetch_20newsgroups(subset='test', categories=cats)


def imdb_data(directory):
    # labels = set([x for x in os.listdir(directory) if os.path.isdir(x)])
    # if 'unsup' in labels:
    #     labels.remove('unsup')
    # labels = list(labels)
    # labels.sort()
    labels = ["neg", "pos"]
    lab_to_num = {}
    for i, l in enumerate(labels):
        lab_to_num[l] = i
    X, y = [], []
    for l in labels:
        path = directory + l
        for filename in os.listdir(path):
            # if '(' in filename:
            #     continue
            f = open(path + '/' + filename, encoding='latin1')
            X.append(f.read())
            f.close()
            y.append(lab_to_num[l])
    return X, y


class BaseTokenizer:
    def __init__(self):
        self.fcode = {}
        self.bcode = {}

    def fit(self, X, y=None):
        xx = self.normalize(X)
        xx = self.tokenize(xx)
        tokens = set()
        for x in xx:
            for w in x:
                tokens.add(w)
        tokens = sorted(list(tokens))
        s = 0
        for token in tokens:
            self.fcode[token] = s
            self.bcode[s] = token
            s += 1
        print("tokenizer dict len: ", len(self.fcode))
        return self

    def transform(self, X, y=None):
        xx = self.normalize(X)
        xx = self.tokenize(xx)
        return [[self.fcode[w] for w in x if w in self.fcode.keys()] for x in xx]

    def decode(self, X, y=None):
        return ["".join([self.bcode[w] for w in x if w in self.bcode.keys()]) for x in X]

    def normalize(self, X):
        return [re.sub("[^\w\d]", "", x).lower() for x in X]

    def tokenize(self, X):
        return [[c for c in x] for x in X]


class WordTokenizer(BaseTokenizer):
    def __init__(self):
        super().__init__()

    def normalize(self, X):
        return [re.sub("[^ \w\d]", "", re.sub(" +", " ", x)).lower() for x in X]

    def tokenize(self, X):
        return [x.split(" ") for x in X]


class CharTokenizer(BaseTokenizer):
    def __init__(self):
        super().__init__()

    def normalize(self, X):
        return [re.sub("[^\w\d]", "", x).lower() for x in X]

    def tokenize(self, X):
        return [[c for c in x] for x in X]


class TokenizerP:
    def __init__(self):
        self.tok = Tokenizer()

    def fit(self, X, y=None):
        xx = [re.sub("[^\w\d]", "", x).lower() for x in X]
        # xx = X
        self.tok.fit(xx)
        return self

    def transform(self, X, y=None):
        xx = [re.sub("[^\w\d]", "", x).lower() for x in X]
        # xx = X
        return self.tok.transform(xx)


class VGramBuilderP:
    def __init__(self, size, iter_num):
        self.vgb = VGramBuilder(size, iter_num)

    def fit(self, X, y=None):
        print("in vgram builder fit")
        self.vgb.fit(X)
        print("in vgram builder fit 2. yeah!")
        return self

    def transform(self, X, y=None):
        res = [" ".join([str(a) for a in x]) for x in self.vgb.transform(X)]
        print("transform")
        return res

    def alpha(self):
        return self.vgb.alphabet()


def only_words(X_tr, y_tr, X_te, y_te):
    pipeline = Pipeline([
        ("words", CountVectorizer()),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    predicted = pipeline.predict(X_te)
    print(np.mean(predicted == y_te))


def only_vgram(X_tr, y_tr, X_te, y_te):
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilderP(15000, 20))
    ])
    data = [x for x in X_tr]
    for x in X_te:
        data.append(x)
    vgram_features.fit(data)
    traint = vgram_features.transform(X_tr)
    testt = vgram_features.transform(X_te)

    pipeline = Pipeline([
        ("words", CountVectorizer()),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=20, random_state=42))
    ])
    pipeline = pipeline.fit(traint, y_tr)
    predicted = pipeline.predict(testt)
    print(np.mean(predicted == y_te))
    # alpha = vgram_features.named_steps['vgb'].alpha()
    # alpha = vgram_features.named_steps['tokenizer'].decode(alpha)
    # for word in alpha:
    #     print(word)


def vgram_and_words(X_tr, y_tr, X_te, y_te):
    size = 5000
    iter = 3
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilderP(size, iter))
    ])
    data = [x for x in X_tr]
    for x in X_te:
        data.append(x)
    vgram_features.fit(data)
    # traint = vgram_features.transform(train.data)
    # testt = vgram_features.transform(test.data)
    all_words = CountVectorizer().fit(data)
    words_without_vgrams = set(all_words.vocabulary_)
    vgrams_alpha = set(vgram_features.named_steps['tokenizer'].decode(vgram_features.named_steps["vgb"].alpha()))
    print("all words: ", len(words_without_vgrams))
    print("intersect: ", len(words_without_vgrams & vgrams_alpha))
    words_without_vgrams -= vgrams_alpha
    print("words without vgrams: ", len(words_without_vgrams))
    # set(vgram_features.named_steps['tokenizer'].decode(vgram_features.named_steps["vgb"].alpha()))

    pipeline = Pipeline([
        ("union", FeatureUnion([
            ("vgrams", Pipeline([
                ("tokenizer", CharTokenizer()),
                ("vgb", VGramBuilderP(size, iter)),
                ("count", CountVectorizer())
            ])),
            ("words", CountVectorizer(vocabulary=words_without_vgrams))])),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=20, random_state=42))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    predicted = pipeline.predict(X_te)
    print(np.mean(predicted == y_te))
    # alpha = vgram_features.named_steps['vgb'].alpha()
    # alpha = vgram_features.named_steps['tokenizer'].decode(alpha)
    # for word in alpha:
    #     print(word)


def all_union(X_tr, y_tr, X_te, y_te):
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilderP(5000, 3)),
        ('vectorizer', CountVectorizer())
    ])
    pipeline = Pipeline([
        ("features", FeatureUnion([("vgram", vgram_features), ("words", CountVectorizer())])),
        # ("words", CountVectorizer()),
        # ("vgram", vgram_features),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    predicted = pipeline.predict(X_te)
    print(np.mean(predicted == y_te))


def get_data(dataset):
    if dataset == "20ng":
        ng_train, ng_test = fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
        X_tr, y_tr, X_te, y_te = ng_train.data, ng_train.target, ng_test.data, ng_test.target
    elif dataset == "imdb":
        imdb_X_tr, imdb_y_tr = imdb_data("/Users/akhvorov/data/datasets/aclImdb/train/")
        imdb_X_te, imdb_y_te = imdb_data("/Users/akhvorov/data/datasets/aclImdb/test/")
        X_tr, y_tr, X_te, y_te = imdb_X_tr, imdb_y_tr, imdb_X_te, imdb_y_te
    else:
        print("wrong dataset")
        return -1
    print("train data shape: ", len(X_tr), len(X_tr[0]), len(y_tr))
    print("test data shape: ", len(X_te), len(X_te[0]), len(y_te))
    return X_tr, y_tr, X_te, y_te


def apply_model(X_tr, y_tr, X_te, y_te, mode):
    if mode == "v":
        only_vgram(X_tr, y_tr, X_te, y_te)
    elif mode == "w":
        only_words(X_tr, y_tr, X_te, y_te)
    elif mode == "w|v" or mode == "v|w":
        all_union(X_tr, y_tr, X_te, y_te)
    elif mode == "w&v" or mode == "v&w":
        vgram_and_words(X_tr, y_tr, X_te, y_te)
    else:
        print("wrong mode")
    # test_20ng()
    # restore_test()
    # all_union(X_tr, y_tr, X_te, y_te)
    # only_vgram(X_tr, y_tr, X_te, y_te)
    # vgram_and_words(X_tr, y_tr, X_te, y_te)


def arg_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument ('-d', '--dataset')
    parser.add_argument ('-m', '--mode')
    # parser.add_argument ('-p', '--path', default='.')
    # parser.add_argument ('-d', '--dataset', default='20newsgroups')
    # parser.add_argument ('-n', '--size', default=15000)
    # parser.add_argument ('-s', '--sparse', default=True)
    # parser.add_argument ('-r', '--restored', default=False)
    # parser.add_argument ('-v', '--use_vgrams', default=True)
    # parser.add_argument ('-w', '--use_words', default=False)
    return parser


def main():
    args = arg_parser().parse_args()
    start = time.time()
    X_tr, y_tr, X_te, y_te = get_data(args.dataset)
    apply_model(X_tr, y_tr, X_te, y_te, args.mode)
    print("time: ", time.time() - start)


if __name__ == "__main__":
    main()


'''
1. 0.8614 - words + vgrams(25000, 20), double intersection, train on train, svm(1e-4, max_iter=50)
'''
