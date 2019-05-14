#! /usr/bin/env python

import numpy as np
from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer
from sklearn.linear_model import SGDClassifier
from sklearn.datasets import fetch_20newsgroups
from sklearn.pipeline import Pipeline
from vgram import *


def get_data():
    return fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')


def test_vgram_svm():
    train, test = get_data()
    data = train.data + test.data
    vgram = Pipeline([
        ("vgb", VGram(15000, 10)),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

    pipeline = Pipeline([
        ("features", vgram),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=3))
    ])
    pipeline.fit(train.data, train.target)
    print("train:", np.mean(pipeline.predict(train.data) == train.target))
    print("test:", np.mean(pipeline.predict(test.data) == test.target))
    alpha = vgram.named_steps["vgb"].alphabet()
    print(alpha[:10])
    assert np.mean(pipeline.predict(train.data) == train.target) > 0.9940
    assert np.mean(pipeline.predict(test.data) == test.target) > 0.8412
    assert alpha[:10] == ['f', 'ff', 'fforg', 'fford', 'ffordwinona', 'ffe', 'ffer', 'fferbertmabell', 'ffey', 'ffair']


def test_vgram_svm_small():
    train, test = get_data()
    data = train.data + test.data
    data = data[:1000]
    vgram = Pipeline([
        ("vgb", VGram(500, 1)),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

    pipeline = Pipeline([
        ("features", vgram),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=3))
    ])
    pipeline.fit(train.data, train.target)
    assert np.mean(pipeline.predict(train.data) == train.target) > 0.75
    assert np.mean(pipeline.predict(test.data) == test.target) > 0.56
    print("train:", np.mean(pipeline.predict(train.data) == train.target))
    print("test:", np.mean(pipeline.predict(test.data) == test.target))
    alpha = vgram.named_steps["vgb"].alphabet()
    assert alpha[:10] == ['f', 'from', 'for', 'forthe', 'fa', 'fe', 'fer', 'fi', 'find', 'r']


def test_save_vgram_small():
    path = "test_dict.json"
    train, test = get_data()
    data = train.data + test.data
    data = data[:1000]
    vgram = VGram(500, 1)
    vgram.fit(data)
    print("fit")
    vgram.save(path)
    print("save")

    pipeline1 = Pipeline([
        ("features", vgram),
        ("vect", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=3))
    ])
    pipeline1.fit(train.data, train.target)
    print("fit 1 pip")

    pipeline2 = Pipeline([
        ("features", loadVGram(path)),
        ("vect", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=3))
    ])
    print("load 2")
    pipeline2.fit(train.data, train.target)
    print("fit 2 pip")

    print(np.mean(pipeline1.predict(train.data) == train.target), np.mean(pipeline2.predict(train.data) == train.target))
    print(np.mean(pipeline1.predict(test.data) == test.target), np.mean(pipeline2.predict(test.data) == test.target))

    assert abs(np.mean(pipeline1.predict(train.data) == train.target) - np.mean(pipeline2.predict(train.data) == train.target)) < 1e-5
    assert abs(np.mean(pipeline1.predict(test.data) == test.target) - np.mean(pipeline2.predict(test.data) == test.target)) < 1e-5


def test_save_int_vgram_small():
    path = "test_dict.json"
    train, test = get_data()
    data = train.data + test.data
    data = data[:1000]
    vgram = Pipeline([
        ("tok", CharTokenizer()),
        ("ivg", IntVGram(500, 1))
    ])
    vgram.fit(data)
    vgram.named_steps["ivg"].save(path)

    pipeline1 = Pipeline([
        ("features", vgram),
        ("vect", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=3))
    ])
    pipeline1.fit(train.data, train.target)

    pipeline2 = Pipeline([
        ("tok", CharTokenizer()),
        ("features", loadIntVGram(path)),
        ("vect", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=3))
    ])
    pipeline2.fit(train.data, train.target)

    print(np.mean(pipeline1.predict(train.data) == train.target), np.mean(pipeline2.predict(train.data) == train.target))
    print(np.mean(pipeline1.predict(test.data) == test.target), np.mean(pipeline2.predict(test.data) == test.target))

    assert abs(np.mean(pipeline1.predict(train.data) == train.target) - np.mean(pipeline2.predict(train.data) == train.target)) < 1e-5
    assert abs(np.mean(pipeline1.predict(test.data) == test.target) - np.mean(pipeline2.predict(test.data) == test.target)) < 1e-5


def test_save_int_stream_vgram():
    path = "test_int_stream_dict.json"
    train, test = get_data()
    data = train.data + test.data
    data = data[:1000]
    tokenizer = CharTokenizer()
    vgram = IntStreamVGram(500)
    for seq in tokenizer.transform(data):
        vgram.accept(seq)
    vgram.update()
    print("fit")
    vgram.save(path)
    print("save")
    loadedVgram = loadIntStreamVGram(path)
    print("load int stream vgram")
    print(loadedVgram)
    print("print loaded vgram")
    print(loadedVgram.parse(tokenizer.transform(data)[0]))
    print("parsed")


def test_save_stream_vgram():
    path = "test_int_stream_dict.json"
    train, test = get_data()
    data = train.data + test.data
    data = data[:1000]
    vgram = StreamVGram(500)
    for seq in data:
        vgram.accept(seq)
    vgram.update()
    print("fit")
    vgram.save(path)
    print("save")
    loadedVgram = loadStreamVGram(path)
    print("load stream vgram")
    print(loadedVgram.parse(data[0]))
    print("parsed")


if __name__ == "__main__":
    test_vgram_svm()
    # test_vgram_svm_small()
    # test_save_vgram_small()
    # test_save_int_vgram_small()

    # test_save_int_stream_vgram()
    # test_save_stream_vgram()
