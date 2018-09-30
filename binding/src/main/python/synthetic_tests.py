import random
import time
import numpy as np
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.pipeline import Pipeline
from sklearn.linear_model import SGDClassifier
from vgram import VGramBuilder


def restore_test():
    alpha = [i for i in range(26)]
    vgb = VGramBuilder(alpha, len(alpha) + 100)
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


def classify_text(pipeline):
    train, test = download_data()

    pipeline = pipeline.fit(train.data, train.target)

    docs_test = test.data
    predicted = pipeline.predict(docs_test)
    print(np.mean(predicted == test.target))


def newsgroups1():
    n_features = 20000
    print('num of features:', n_features)
    cats = None
    baseline = Pipeline([('vect', CountVectorizer()),
                         ('tfidf', TfidfTransformer()),
                         # ('kbest', SelectKBest(k=n_features)),
                         (('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-3, n_iter=5, random_state=42)))
                         ])
    '''vgramm = Pipeline([('vect', CountVectorizer(vocabulary=voc)),
                       ('tfidf', TfidfTransformer()),
                       ('kbest', SelectKBest(k=n_features)),
                       (('clf', LinearSVC()))
                       ])'''
    classify_text(baseline)


class Tokenizer:
    def __init__(self):
        self.fcode = {}
        self.bcode = {}
        self.size = 0

    # def encode(self, doc, sep=' '):
    #     seq = []
    #     for word in doc.split(sep):
    #         if word not in map:
    #             self.fcode[word] = self.size
    #             self.bcode[self.size] = word
    #             self.size += 1
    #         seq.append(self.fcode[word])
    #     return seq

    def encode(self, docs, sep=''):
        res = []
        for doc in docs:
            seq = []
            if sep == '':
                words = doc
            else:
                words = doc.split(sep)
            for word in words:
                if word not in self.fcode:
                    self.fcode[word] = self.size
                    self.bcode[self.size] = word
                    self.size += 1
                seq.append(self.fcode[word])
            res.append(seq)
        return res

    def decode(self, docs):
        res = []
        for doc in docs:
            seq = ""
            for symb in doc:
                seq += self.bcode[symb]
            res.append(seq)
        return res

    def coded_format(self, seqs):
        res = []
        for seq in seqs:
            doc = ' '.join(['_'.join([str(i) for i in word]) for word in seq])
            res.append(doc)
        return res


def alpha_info(vgb, tokenizer):
    alpha0 = vgb.alphabet()
    print("alpha size: ", len(alpha0))
    alpha = tokenizer.decode(alpha0)
    for i in range(1000):
        print(alpha0[i], " -- ", alpha[i])


def do_ml(X_tr, y_tr, X_te, y_te):
    word_vectorizer1 = CountVectorizer()
    word_vectorizer1.fit(X_tr)
    pipeline = Pipeline([('vect', CountVectorizer(vocabulary=word_vectorizer1.vocabulary)),
                         ('tfidf', TfidfTransformer()),
                         (('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-3, max_iter=5, random_state=42)))
                         ])
    pipeline = pipeline.fit(X_tr, y_tr)
    predicted = pipeline.predict(X_te)
    print(np.mean(predicted == y_te))


def test_20ng():
    start = time.time()
    train, test = fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
    print("dataset: ", time.time() - start)
    tokenizer = Tokenizer()
    ttrain = tokenizer.encode(train.data)
    ttest = tokenizer.encode(test.data)
    print("encode: ", time.time() - start)
    vgb = VGramBuilder(1000)
    for k in range(10):
        for i in range(len(ttrain)):
            vgb.accept(ttrain[i])
        for i in range(len(ttest)):
            vgb.accept(ttest[i])
    print("accept: ", time.time() - start)

    #alpha_info(vgb, tokenizer)
    alpha = vgb.alphabet()

    ttrain_v = tokenizer.coded_format([[alpha[i] for i in vgb.parse(doc) if i < len(alpha)] for doc in ttrain])
    ttest_v = tokenizer.coded_format([[alpha[i] for i in vgb.parse(doc) if i < len(alpha)] for doc in ttest])
    ttrain_w = tokenizer.coded_format([tokenizer.encode(doc.split(' ')) for doc in train])
    ttest_w = tokenizer.coded_format([tokenizer.encode(doc.split(' ')) for doc in test])
    print("format: ", time.time() - start)

    # do_ml(ttrain, train.target, ttest, test.target)
    # print("ml: ", time.time() - start)

    vectorizer_v1 = CountVectorizer()
    vectorizer_v1.fit(ttrain_v)
    vectorizer_w1 = CountVectorizer()
    vectorizer_w1.fit(ttrain_w)

    vectorizer_w2 = CountVectorizer(vocabulary=set(vectorizer_w1.vocabulary_.keys()) - set(vectorizer_v1.vocabulary_.keys()))
    vec_ttrain_w = vectorizer_w2.fit_transform(ttrain_w)
    vec_ttrain_v = vectorizer_v1.transform(ttrain_v)
    vec_ttrain = np.hstack((vec_ttrain_w, vec_ttrain_v))

    vec_ttest_w = vectorizer_w2.fit_transform(ttest_w)
    vec_ttest_v = vectorizer_v1.transform(ttest_v)
    vec_ttest = np.hstack((vec_ttest_w, vec_ttest_v))
    print("vectorizer: ", time.time() - start)

    pipeline = Pipeline([('tfidf', TfidfTransformer()),
                         ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-3, max_iter=5, random_state=42))
                         ])
    pipeline = pipeline.fit(vec_ttrain, train.target)
    predicted = pipeline.predict(vec_ttest)
    print(np.mean(predicted == test.target))

    print("ml: ", time.time() - start)




    # print(type(train))
    # print(train.data[0])
    # vectorizer = CountVectorizer(analyzer='char')
    # train_char = vectorizer.fit_transform(train.data)
    # print('##################')
    # print(train_char[0])
    # print('##################')
    # print(vectorizer.decode(train.data[0]))


def main():
    start = time.time()
    test_20ng()
    print("time: ", time.time() - start)


if __name__ == "__main__":
    main()
