import os
from pathlib import Path
from sklearn.datasets import fetch_20newsgroups


def imdb_data(directory, unsup=False):
    X = []
    if unsup:
        src = Path(directory)
        for filename in src.rglob('*.txt'):
            f = open(str(filename), encoding='latin1')
            X.append(f.read())
            f.close()
        return X
    labels = sorted(["neg", "pos"])
    lab_to_num = {}
    for i, l in enumerate(labels):
        lab_to_num[l] = i
    y = []
    names = []
    for l in labels:
        path = directory + l
        for filename in os.listdir(path):
            names.append(path + '/' + filename)
            y.append(lab_to_num[l])
    names = sorted(names)
    for name in names:
        f = open(name, encoding='latin1')
        X.append(f.read())
        f.close()
    return X, y


def get_imdb():
    imdb_X_tr, imdb_y_tr = imdb_data("/Users/akhvorov/data/datasets/aclImdb/train/")
    imdb_X_te, imdb_y_te = imdb_data("/Users/akhvorov/data/datasets/aclImdb/test/")
    return imdb_X_tr, imdb_X_te, imdb_y_tr, imdb_y_te


def get_20ng():
    train = fetch_20newsgroups(subset='train')
    test = fetch_20newsgroups(subset='test')
    return train.data, test.data, train.target, test.target
