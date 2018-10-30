import argparse
import os
import random
import re
import csv
import time
from pathlib import Path
from vgram import VGramBuilder, CharTokenizer

import numpy as np
from scipy.stats import poisson
from sklearn.model_selection import train_test_split
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.linear_model import SGDClassifier
from sklearn.pipeline import Pipeline, FeatureUnion

import string
from scipy.sparse import spmatrix, coo_matrix

from sklearn.base import BaseEstimator
from sklearn.linear_model.base import LinearClassifierMixin, SparseCoefMixin
from sklearn.svm import LinearSVC

from catboost import CatBoostClassifier


def restore_test():
    alpha = [chr(i + ord('a')) for i in range(26)]
    vgb = VGramBuilder(len(alpha) + 100, 1)
    X = []
    for i in range(1000):
        l = random.randint(1, 300)
        seq = "".join(alpha[random.randint(0, len(alpha) - 1)] for _ in range(l))
        X.append(seq)
    vgb.fit(CharTokenizer().fit(X).transform(X))
    print(str(len(vgb.alphabet())) + ' < 31')


def restore_rand():
    al = [chr(i + ord('a')) for i in range(26)]
    res = []
    for i in range(10):
        known = set()
        models = list(filter(lambda x: x not in known, ["".join(al[k] for k in range(poisson.rvs(5) + 1)) for j in range(1000)]))
        known = known | set(models)
        alphabet = set("".join(model for model in models))
        vgb = VGramBuilder(len(models) + len(alphabet), 10)
        X = []
        for j in range(100000):
            l = random.randint(1, 100)
            X.append("".join(models[random.randint(0, len(models) - 1)] for _ in range(l)))
        tok = CharTokenizer().fit(X)
        vgb.fit(tok.transform(X))
        alpha = tok.decode(vgb.alphabet())
        alpha = set(alpha) - alphabet
        known = known - alpha
        rate = len(known) / len(models)
        res.append(rate)
        print("errors: " + str(len(known) / len(models)))
    print(res)
    print(np.mean(np.array(res)))


class NBSVM(BaseEstimator, LinearClassifierMixin, SparseCoefMixin):
    def __init__(self, norm_coef=1e-5, alpha=1, C=1, beta=0.25, fit_intercept=False, iters=10000):
        self.alpha = alpha
        self.C = C
        self.beta = beta
        self.fit_intercept = fit_intercept
        self.norm_coef = norm_coef
        self.iters = iters

    def fit(self, X, y):
        self.classes_ = np.unique(y)
        if len(self.classes_) == 2:
            coef_, intercept_ = self._fit_binary(X, y)
            self.coef_ = coef_
            self.intercept_ = intercept_
        else:
            coef_, intercept_ = zip(*[
                self._fit_binary(X, y == class_)
                for class_ in self.classes_
            ])
            self.coef_ = np.concatenate(coef_)
            self.intercept_ = np.array(intercept_).flatten()
        return self

    def _fit_binary(self, X, y):
        p = np.asarray(self.alpha + X[y == 1].sum(axis=0)).flatten()
        q = np.asarray(self.alpha + X[y == 0].sum(axis=0)).flatten()
        r = np.log(p/np.abs(p).sum()) - np.log(q/np.abs(q).sum())
        b = np.log((y == 1).sum()) - np.log((y == 0).sum())

        if isinstance(X, spmatrix):
            indices = np.arange(len(r))
            r_sparse = coo_matrix(
                (r, (indices, indices)),
                shape=(len(r), len(r))
            )
            X_scaled = X * r_sparse
        else:
            X_scaled = X * r

        lsvc = LinearSVC(
            C=self.C,
            fit_intercept=self.fit_intercept,
            max_iter=10000
        ).fit(X_scaled, y)
        # print("alpha:", self.norm_coef, "iter:", self.iters)
        # if self.norm_coef < 1e-7:
        #     print("without regularization")
        #     lsvc = SGDClassifier(loss='hinge', penalty="none", max_iter=self.iters, random_state=42)
        # else:
        #     print("with regularization")
        #     lsvc = SGDClassifier(loss='hinge', penalty='l2', alpha=self.norm_coef, max_iter=self.iters, random_state=42)
        # lsvc.fit(X_scaled, y)

        mean_mag =  np.abs(lsvc.coef_).mean()

        coef_ = (1 - self.beta) * mean_mag * r + \
                self.beta * (r * lsvc.coef_)

        intercept_ = (1 - self.beta) * mean_mag * b + \
                     self.beta * lsvc.intercept_

        return coef_, intercept_


def load_imdb(X_tr, X_te, vgb):
    print("Vectorizing Training Text")
    token_pattern = r'\w+|[%s]' % string.punctuation
    ngram_vectorizer = CountVectorizer(#ngram_range=(1, 2),
        token_pattern=token_pattern,
        binary=True)
    unigram_vectorizer = CountVectorizer(
        token_pattern=token_pattern,
        binary=True)
    # bigram_vectorizer = CountVectorizer(ngram_range=(2, 2),
    #     token_pattern=token_pattern,
    #     binary=True)
    if vgb is None:
        print("only words")
        # features = FeatureUnion([("uni", Pipeline([("uno", unigram_vectorizer), ("tfidf", TfidfTransformer(sublinear_tf=True))])), ("bi", bigram_vectorizer)])
        features = ngram_vectorizer
        # features = Pipeline([("fu", ngram_vectorizer), ("tfidf", TfidfTransformer())])
    else:
        # print("Using both")
        print("words with vgrams")
        # features = FeatureUnion([("vgb", vgb), ("ngrams", ngram_vectorizer)])
        # features = Pipeline([("vgb", vgb), ("tfidf", TfidfTransformer)])
        features = Pipeline([("grams", FeatureUnion([("vgb", vgb), ("unigrams", unigram_vectorizer)])), ("tfidf", TfidfTransformer(sublinear_tf=True))])
        # features = FeatureUnion([Pipeline([("vgb", vgb), ("tfidf", TfidfTransformer(sublinear_tf=True))]),
        #                          ("ngrams", ngram_vectorizer)])
    # pipeline = Pipeline([("fu", features), ("tfidf", TfidfTransformer())])
    X_tr = features.fit_transform(X_tr)
    X_te = features.transform(X_te)
    return X_tr, X_te


def nbsvm(X_tr, y_tr, X_te, y_te, vgb=None, alpha=1e-5):
    X_tr, X_te = load_imdb(X_tr, X_te, vgb)
    y_tr = np.array(y_tr, dtype=int)
    y_te = np.array(y_te, dtype=int)
    print("Fitting Model")
    mnbsvm = NBSVM(norm_coef=1e-4, iters=1000)#alpha)
    mnbsvm.fit(X_tr, y_tr)
    print('Train Accuracy: %s' % mnbsvm.score(X_tr, y_tr))
    print('Test Accuracy: %s' % mnbsvm.score(X_te, y_te))


def format_dict_java_to_cpp(from_file, to_file, tokenizer):
    out = open(to_file, 'w')
    for line in open(from_file, 'r'):
        line_num = line.split('\t')
        if len(line_num) > 0:
            num = line_num[-1][:-1]
            word = line[: -len(line_num[-1]) - 1]
            vec = tokenizer.transform([word])[0]
            if len(vec) > 0:
                out.write(num + "\t(" + " ".join(map(str, vec)) + ")\t" + word + "\n")
    out.close()


def format_dict_cpp_to_java(from_file, to_file, tokenizer):
    out = open(to_file, 'w')
    for line in open(from_file, 'r'):
        line_num = line.split('\t')
        if len(line_num) < 1:
            print("error in", line)
            return
        num = line_num[0]
        beg = line.find('(') + 1
        end = line.find(')')
        vec = list(map(int, line[beg:end].split(' ')))
        word = tokenizer.decode([vec])[0]
        out.write(word + "\t" + num + "\n\n")
    out.close()


def translate_dict(directory, from_file, to_file, domain):
    tokenizer = CharTokenizer()
    data = imdb_data(directory, unsup=True)
    tokenizer.fit(data)
    if domain == "cpp":
        format_dict_cpp_to_java(from_file, to_file, tokenizer)
    elif domain == "java":
        format_dict_java_to_cpp(from_file, to_file, tokenizer)
    else:
        print("wrong domain: only cpp and java allowed")


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
            # f = open(path + '/' + filename, encoding='latin1')
            # X.append(f.read())
            # f.close()
            y.append(lab_to_num[l])
    names = sorted(names)
    for name in names:
        f = open(name, encoding='latin1')
        X.append(f.read())
        f.close()
    return X, y


def ag_news_data(directory, unsup=False):
    XX, yy = [], []
    files = ["train", "test"]
    for i in range(2):
        X, y = [], []
        with open(directory + "/" + files[i] + ".csv", 'r') as f:
            rdr = csv.reader(f, delimiter=',', quotechar='"')
            for row in rdr:
                txt = ""
                for s in row[1:]:
                    txt = txt + " " + re.sub("^\s*(.-)\s*$", "%1", s).replace("\\n", "\n")
                X.append(txt)
                y.append(int(row[0]))
        XX.append(X)
        yy.append(y)
    if unsup:
        return XX[0] + XX[1]
    return XX[0], yy[0], XX[1], yy[1]


class VGramBuilderP:
    def __init__(self, size=None, iter=None, filename=None):
        if filename is not None:
            self.vgb = VGramBuilder(filename)
            self.fitted = True
        else:
            self.vgb = VGramBuilder(size, iter)
            self.fitted = False

    def fit(self, X, y=None):
        print("in vgram builder fit start")
        start = time.time()
        if not self.fitted:
            self.vgb.fit(X)
            self.fitted = True
        print("in vgram builder fit end, time:", time.time() - start)
        return self

    def transform(self, X, y=None):
        xx = self.vgb.transform(X)
        for i in range(len(xx)):
            if len(xx[i]) == 0:
                print("empty: {}, !{}!, !{}!".format(i, X[i], xx[i]))
        res = [" ".join([str(a) for a in x]) for x in xx]
        # print("transform")
        return res

    def alpha(self):
        return self.vgb.alphabet()

    def save(self, filename, tokenizer=None):
        freqs = self.vgb.freqs()
        alpha = self.vgb.alphabet()
        f = open(filename, 'w')
        i = 0
        for (freq, seq) in zip(freqs, alpha):
            f.write(str(freq) + "\t(" + " ".join(map(str, seq)) + ")")
            if tokenizer is not None:
                text = tokenizer.decode([seq])[0]
                f.write("\t" + text)
            f.write("\n")
            i += 1
        f.close()


def save_bin_to_file(X, Y, filename):
    with open(filename, 'w') as file:
        for (x, y) in zip(X, Y):
            file.write(str(y) + "\t")
            for i in range(len(x)):
                file.write(str(x[i]))
            file.write("\n")


def read_bin_from_file(filename):
    X, Y = [], []
    with open(filename, 'r') as file:
        for line in file:
            yx = line.split('\t')
            # print(len(yx[0]), len(yx[1]))
            Y.append(int(yx[0]))
            res = [int(yx[1][i]) for i in range(len(yx[1])) if yx[1][i] != '\n']
            # print(res)
            X.append(res)
    return np.array(X), np.array(Y)


def boosting(X_tr, y_tr, X_te, y_te):
    model = CatBoostClassifier(
        iterations=100,
        # learning_rate=0.001,
        thread_count=4,
        custom_loss=['Accuracy'],
        loss_function='MultiClass'
    )
    model.fit(X_tr, y_tr, use_best_model=True, eval_set=(X_te, y_te), silent=True, plot=True)
    print("Train:", np.mean(model.predict(X_tr) == np.array(y_tr)))
    print("Test:", np.mean(model.predict(X_te) == np.array(y_te)))


def get_data(dataset, unsup=False):
    if dataset[:4] == "20ng":
        if dataset == "20ng":
            ng_train, ng_test = fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
        elif dataset == "20ng_h":
            remove=('headers',)#, 'footers', 'quotes')
            ng_train, ng_test = fetch_20newsgroups(subset='train', remove=remove), fetch_20newsgroups(subset='test', remove=remove)
        else:
            print("error in 20ng dataset name")
        if unsup:
            data = [x for x in ng_train.data]
            for x in ng_test.data:
                data.append(x)
            return data
        X_tr, y_tr, X_te, y_te = ng_train.data, ng_train.target, ng_test.data, ng_test.target
    elif dataset == "imdb":
        if unsup:
            return imdb_data("/Users/akhvorov/data/datasets/aclImdb/", unsup=True)
        imdb_X_tr, imdb_y_tr = imdb_data("/Users/akhvorov/data/datasets/aclImdb/train/")
        imdb_X_te, imdb_y_te = imdb_data("/Users/akhvorov/data/datasets/aclImdb/test/")
        X_tr, y_tr, X_te, y_te = imdb_X_tr, imdb_y_tr, imdb_X_te, imdb_y_te
    elif dataset == "mpqa":
        X_neg = [l for l in open("/Users/akhvorov/data/datasets/manning_data/mpqa/mpqa.neg") if l]
        X_pos = [l for l in open("/Users/akhvorov/data/datasets/manning_data/mpqa/mpqa.pos") if l]
        X = X_neg + X_pos
        if unsup:
            return X
        y = np.array([0] * len(X_neg) + [1] * len(X_pos))
        X_tr, X_te, y_tr, y_te = train_test_split(X, y)
        return X_tr, y_tr, X_te, y_te
    elif dataset == "agn":
        return ag_news_data("/Users/akhvorov/data/datasets/ag_news", unsup)
    else:
        print("wrong dataset")
        return -1
    print("train data shape: ", len(X_tr), len(X_tr[0]), len(y_tr))
    print("test data shape:  ", len(X_te), len(X_te[0]), len(y_te))
    return X_tr, y_tr, X_te, y_te


def vgb_pipeline(args):
    if args.restore:
        vgb = VGramBuilder(args.dict_path)
    else:
        vgb = VGramBuilder(int(args.size), int(args.iter))
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", vgb),
        ("vect", CountVectorizer())
    ])
    data = get_data(args.dataset, unsup=True)
    print("texts for vgram learning:", len(data))
    vgram_features.fit(data)
    # alpha = vgram_features.named_steps["tokenizer"].decode(vgram_features.named_steps["vgb"].alphabet())
    # res = vgram_features.named_steps['vgb'].transform(vgram_features.named_steps['tokenizer'].transform(["reakableottomofthe"]))
    # print(res)
    # print([alpha[int(i)] for i in res[0].strip().split(' ')])
    if not args.restore:
        print("save dict in vgb_pipeline")
        vgram_features.named_steps['vgb'].save(args.dict_path, vgram_features.named_steps['tokenizer'])
    return vgram_features


def apply_model(X_tr, y_tr, X_te, y_te, args):
    print("mode: ", end='')
    if args.mode == "v":
        print("only vgram")
        features = vgb_pipeline(args)
    elif args.mode == "w":
        print("only words")
        features = CountVectorizer()
    elif args.mode == "wv" or args.mode == "vw":
        print("w and v, union of two feature sets")
        vgb = vgb_pipeline(args)
        features = FeatureUnion([("vgb", vgb), ("words", CountVectorizer())])
    else:
        print("wrong mode")
        return

    alpha = 1e-4  # float(args.alpha)
    iters = 100
    print("alpha:", alpha, "iter:", iters)
    pipeline = Pipeline([
        ("features", features),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        # ('clf', SGDClassifier(loss='hinge', penalty='none', max_iter=iters, random_state=42))
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=alpha, max_iter=iters, random_state=42))
        # ('clf', LinearSVC(C=1, fit_intercept=False, max_iter=10000))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    print("train accuracy: ", np.mean(pipeline.predict(X_tr) == y_tr))
    predicted = pipeline.predict(X_te)
    print("test accuracy: ", np.mean(predicted == y_te))


def arg_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dataset')
    parser.add_argument('-m', '--mode')
    parser.add_argument('-s', '--size', default=15000)
    parser.add_argument('-i', '--iter', default=15)
    parser.add_argument('-r', '--restore', action='store_true')
    parser.add_argument('-t', '--tokenizer', default="1")
    parser.add_argument('-ns', '--nbsvm', action='store_true')
    parser.add_argument('-a', '--alpha', default=1e-4)
    parser.add_argument('-dp', '--dict_path')
    parser.add_argument('-b', '--boost', action='store_true')
    return parser


def main():
    args = arg_parser().parse_args()
    start = time.time()
    X_tr, y_tr, X_te, y_te = get_data(args.dataset)
    if args.boost:
        print("Algorithm: Boosting")
        vgb = vgb_pipeline(args)
        x_tr = vgb.transform(X_tr).toarray()
        x_te = vgb.transform(X_te).toarray()
        print("Shape:", x_tr.shape, x_te.shape)
        boosting(x_tr, y_tr, x_te, y_te)
    else:
        if args.nbsvm:
            print("Algorithm: NBSVM")
            if args.mode != "w":
                vgb = vgb_pipeline(args)
            nbsvm(X_tr, y_tr, X_te, y_te, vgb, float(args.alpha))
        else:
            print("Algorithm: SVM")
            apply_model(X_tr, y_tr, X_te, y_te, args)
    print("time: ", time.time() - start)


if __name__ == "__main__":
    main()
    # restore_rand()
    # restore_test()


'''
https://docs.google.com/spreadsheets/d/1NGXZBGn_4j8sLQaCMM79GFHX4fOLgQnqM-fZ4gXa22U/edit#gid=0

78. (0.8641, 0.9955) -d 20ng -m wv -s 15000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=50))
77. (0.8643, 0.9957) -d 20ng -m wv -s 15000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=100))
76. (0.8638, 0.9957) -d 20ng -m v -s 15000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=500)) # что-т не так
75. (0.8419, 0.9949) -d 20ng -m v -s 15000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=50))
74. (0.8635, 0.9957) -d 20ng -m w (sgd(alpha=1e-4, iter=50))

73. (0.8967, 0.9512) -d imdb -m wv -s 15000 -i 15 (new_vgram, sgd(alpha=1e-4, iter=100))
72. (0.8968, 0.9592) -d imdb -m wv -s 50000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=50))
71. (0.8969, 0.9593) -d imdb -m wv -s 50000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=100))
70. (0.8891, 0.9580) -d imdb -m v -s 50000 -i 50 (new_vgram, sgd(alpha=1e-4, iter=50))
69. (0.8921, 0.9439) -d imdb -m v -s 15000 -i 15 (new_vgram, sgd(alpha=1e-4, iter=500))
68. (0.8947, 0.9456) -d imdb -m v -s 15000 -i 15 (new_vgram, sgd(alpha=1e-4, iter=50))
67. (0.8889, 0.9445) -d imdb -m w (sgd(alpha=1e-4, iter=50))

Лажа, так как без регуляризации SGD был
# 77. (0., 0.) -d 20ng -m vw -s 15000 -i 3 -t 1 -r (sgd(alpha=4e-4, iter=1000))
# 76. (0.8375, 0.9995) -d 20ng -m vw -s 15000 -i 50 -t 1 -r (sgd(alpha=4e-4, iter=1000))
# 75. (0.8979, 0.9510) -d imdb -m vw -s 15000 -i 15 -r (sgd(alpha=4e-4, iter=1000))
# 74. (0.8925, 0.9436) -d imdb -m v -s 15000 -i 15 -r (sgd(alpha=1e-4, iter=1000))
# 73. (0.8889, 0.9445) -d imdb -m w (sgd(alpha=1e-4, iter=1000))
# 
# 72. (0.8659, 0.9954) -d 20ng -m wv -s 15000 -i 50 -r (sgd(alpha=1e-4, iter=1000))
# 71. (0.8635, 0.9957) -d 20ng -m w (sgd(alpha=1e-4, iter=1000))
# 70. (0.8455, 0.9948) -d 20ng -m v -s 15000 -i 50 -r (sgd(alpha=1e-4, iter=1000))
# 69. (0.8656, 0.9988) -d 20ng -m vw -s 15000 -i 50 -r (sgd(alpha=5e-5, iter=10000)) - win 5
# 68. (0.8352б 0.9981) -d 20ng -m v -s 15000 -i 50 -r (sgd(alpha=5e-5, iter=10000))
# 67. (0.8600, 0.9988) -d 20ng -m w (sgd(alpha=5e-5, iter=10000))

66(58). (0.8806, 0.9997) -d imdb -m wv -s 50000 -i 50 -t 1 (tfidf, sgd(alpha=1e-5, iter=10000))
--42. (0.8652, 0.9101) -d imdb -m v -s 15000 -i 6 -t 1 -r -ns (nbsvm, non-tfidf)
65(58). (0.9040, 0.9634) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=0, iter=20000))
64(57). (0., 0.) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=1e-5)) - win 3
63. (0.9020, 0.9699) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=5e-6))
--43. (0.8615, 0.9949) -d 20ng -m vw -s 15000 -i 6 -t 1 (imdb dict, sublinear_tf=True)
62(54). (0.8989, 0.9753) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=3e-6))
61. (0., 0.) -d imdb -m wv -s 15000 -i 15 -t 1 (nbsvm, tfidf, sgd(alpha=1e-5))
60. (0.8972, 0.9680) -d imdb -m vw -s 25000 -i 20 -r (nbsvm, tfidf, sgd(alpha=3e-6))
59(55). (0.8928, 0.9879) -d imdb -m vw -s 50000 -i 50 -r (sublinear_tf=True, sgd(alpha=3e-6))
58. (0.9038, 0.9506) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=2e-5))
57(53). (0.9049, 0.9610) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf, sgd(alpha=1e-5))

56(2,7). (0.8931, 0.9452) -d imdb -m v -s 15000 -i 3 -t 1 -r (sublinear_tf=True)
55(7). (0.8980, 0.9591) -d imdb -m vw -s 50000 -i 50 -r (sublinear_tf=True)
54. (0.8926, 0.9192) -d imdb -m wv -s 50000 -i 50 -t 1 (nbsvm, tfidf)
53(35). (0.9030, 0.9476) -d imdb -m wv -s 15000 -i 15 -t 1 (nbsvm, tfidf)
52(45). (0.9050, 0.9517) -d imdb -m wv -s 25000 -i 20 -t 1 (nbsvm, tfidf)
51(48). (0.8843, 0.8994) -d imdb -m wv -s 15000 -i 6 -t 1 -r -ns (nbsvm, tfidf, sgd(alpha=2e-4, iter=10000))
50. (0.8985, 0.9515) -d imdb -m wv -s 15000 -i 6 -t 1 -r (sgd(iter=10000))
49. (0.8986, 0.9515) -d imdb -m wv -s 15000 -i 6 -t 1 -r (sgd(iter=500)) - can be better, use 25k dict
48(35). (0.8947, 0.9154) -d imdb -m wv -s 15000 -i 6 -t 1 -r -ns (nbsvm, tfidf, sgd(iter=5000))
47. (0.8944, 0.9158) -d imdb -m wv -s 15000 -i 6 -t 1 -r -ns (nbsvm, tfidf, sgd(iter=500))
46(17). (0.8984, 0.9514) -d imdb -m vw -s 15000 -i 6 -t 1 (sublinear_tf=True, sgd(iter=500))
45. (0.9044, 0.9521) -d imdb -m wv -s 25000 -i 10 -t 1 -r (nbsvm, tfidf)
44. (0.8916, 0.9398) -d imdb -m wv -s 15000 -i 50 -t 1 -r (20ng dict, nbsvm, tfidf)
43. (0.8615, 0.9949) -d 20ng -m vw -s 15000 -i 6 -t 1 (imdb dict, sublinear_tf=True)
42. (0.8652, 0.9101) -d imdb -m v -s 15000 -i 6 -t 1 -r -ns (nbsvm, non-tfidf)

41. (0.8658, 0.9761) -d mpqa -m wv -s 1470 -i 20 -t 1 (nbsvm, non-tfidf)
40. (0.8563, 0.9454) -d mpqa -m w (nbsvm non-tfidf)
39. (0.8556, 0.9121) -d mpqa -m wv -s 1470 -i 20 -t 1 (nbsvm какая-то версия)
38. (0.8303, 0.8945) -d mpqa -m v -s 1470 -i 20 -t 1
37. (0.8284, 0.8602) -d mpqa -m v -s 746 -i 10 -t 1

36. (0.9119, 1.0) -d imdb -m w12 (nbsvm, tfidf only for unigram)
35. (0.9029, 0.9472) -d imdb -m wv -s 15000 -i 6 -t 1 -r (nbsvm, tfidf)
34. (0.8874, 0.9484) -d imdb -m w1 (nbsvm, tfidf)
33. (0.8670, 0.9708) -d imdb -m w1 (nbsvm, non-tfidf)

32. (0.9199, 1.0) -d imdb -m w(1,3)v -s 25000 -i 10 -t 1 -r (nbsvm, non-tfidf)
31. -(0., 0.) -d imdb -m w(1,3)v -s 15000 -i 6 -t 1 -r (nbsvm, tfidf only for vgram)
30. ~(0.9099, 0.9412) -d imdb -m w(1,3)v -s 15000 -i 6 -t 1 -r (nbsvm, tfidf for everything)
29. (0.9093, 0.9386) -d imdb -m w(1,3) (nbsvm)
30. (0.9109, 0.9404) -d imdb -m w(1,3)v -s 15000 -i 6 -t 1 -r (nbsvm, sublinear_tf=True)
29. (0.9093, 0.9386) -d imdb -m w(1,3) (nbsvm, sublinear_tf=True)
28. (0.9205, 1.0) -d imdb -m w(1,3)v -s 15000 -i 6 -t 1 -r (nbsvm, non-tfidf)
27. (0.9137, 1.0) -d imdb -m w(1,2)v -s 15000 -i 6 -t 1 -r (nbsvm, non-tfidf)
26. (0.8980, 0.9552) -d imdb -m vw -s 25000 -i 7 -t 1 (sublinear_tf=True)
25. (0.9138, 1.0) -d imdb -m w(1,2) (nbsvm, non-tfidf)
24. (0.9203, 1.0) -d imdb -m w(1,3) (nbsvm, non-tfidf)

23. (0.8415, 0.9942) -d 20ng -m v -s 15000 -i 50 -t 2 -r (sublinear_tf=True)
22. (0.8630, 0.9956) -d 20ng -m vw -s 15000 -i 50 -t 2 (sublinear_tf=True)
21. (0.8445, 0.9948) -d 20ng -m v -s 15000 -i 50 -t 1 -r (sublinear_tf=True)
20. (0.8662, 0.9955) -d 20ng -m vw -s 15000 -i 50 -t 1 (sublinear_tf=True)
19. (0.8935, 0.9444) -d imdb -m v -s 15000 -i 6 -t 1 (sublinear_tf=True)
18. (0.8917, 0.9435) -d imdb -m v -s 15000 -i 6 -t 2 (sublinear_tf=True)
17. (0.8984, 0.9514) -d imdb -m vw -s 15000 -i 6 -t 1 (sublinear_tf=True)
16. (0.8964, 0.9503) -d imdb -m vw -s 15000 -i 6 -t 2 (sublinear_tf=True)
15. (0.8412, 0.9940) -d 20ng -m v -s 15000 -i 10 -t 1 -r (sublinear_tf=True)
14. (0.8678, 0.9955) -d 20ng -m vw -s 15000 -i 10 -t 1 -r (sublinear_tf=True)
13. (0.8926, 0.9434) -d imdb -m v -s 15000 -i 3 -t 2 (sublinear_tf=True)
12. (0.8967, 0.9369) -d imdb -m vw -s 15000 -i 3 -t 2 (sublinear_tf=True)
11. 0.8614 -d 20ng -m vw -s 15000 -i 10 -r (sublinear_tf=True)  
10. 0.8633 -d 20ng -m w (sublinear_tf=True)  
9. 0.8942 -d imdb -m vw -s 15000 -i 10 (sublinear_tf=True, CharTokenizer2) # wrong, recompute
8. 0.8891 -d imdb -m w (sublinear_tf=True)
7. 0.8979 -d imdb -m vw -s 15000 -i 10 -r (sublinear_tf=True)

-------

6. 0.8591 -d 20ng -m vw -s 15000 -i 10 -r
5. 0.8297 -d 20ng -m v -s 15000 -i 10
4. 0.8538 -d 20ng -m w

3. 0.8937 -d imdb -m vw -s 15000 -i 3 -r
2. 0.8887 -d imdb -m v -s 15000 -i 3
1. 0.8856 -d imdb -m w 

Fail detected 

08.10
8. 0.8918 -d imdb -m v_or_w -s 20000 -i 25
7. 0.8922 -d imdb -m v_or_w -s 15000 -i 15
6. 0.8555 -d 20ng -m v_and_w -s 15000 -i 15
5. 0.8870 -d imdb -m v -s 15000 -i 15
4. 0.8858 -d imdb -m w
3. 0.8538 -d 20ng -m w
2. 0.8578 -d 20ng -m v_or_w -s 15000 -i 15

later
1. 0.8614 - words + vgrams(25000, 20), double intersection, train on train, svm(1e-4, max_iter=50)
'''
