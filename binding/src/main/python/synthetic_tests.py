import random
import time
import re
import os
from pathlib import Path
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
    labels = ["neg", "pos"]
    lab_to_num = {}
    for i, l in enumerate(labels):
        lab_to_num[l] = i
    y = []
    for l in labels:
        path = directory + l
        for filename in os.listdir(path):
            f = open(path + '/' + filename, encoding='latin1')
            X.append(f.read())
            f.close()
            y.append(lab_to_num[l])
    return X, y


class BaseTokenizer:
    def __init__(self):
        self.fcode = {}
        self.bcode = {}

    # def fit(self, X, y=None):
    #     xx = self.normalize(X)
    #     xx = self.tokenize(xx)
    #     tokens = set()
    #     for x in xx:
    #         for w in x:
    #             tokens.add(w)
    #     tokens = sorted(list(tokens))
    #     s = 0
    #     for token in tokens:
    #         self.fcode[token] = s
    #         self.bcode[s] = token
    #         s += 1
    #     print("tokenizer dict len: ", len(self.fcode))
    #     return self

    def fit(self, X, y=None):
        xx = self.normalize(X)
        xx = self.tokenize(xx)
        s = 0
        for x in xx:
            for token in x:
                if token not in self.fcode:
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


# deprecated
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
        res = [" ".join([str(a) for a in x]) for x in self.vgb.transform(X)]
        # print("transform")
        return res

    def alpha(self):
        return self.vgb.alphabet()

    def save(self, filename):
        return self.vgb.save(filename)


def write_vgram_bows_to_file(pipeline, directory):
    alpha = pipeline.named_steps['vgb'].alpha()
    alpha = pipeline.named_steps['tokenizer'].decode(alpha)
    src = Path(directory)
    dst = Path(directory + "-cpp-bows")
    for file in src.rglob('*.txt'):
        dst_file = dst / file.relative_to(src)
        try:
            dst_file.parent.mkdir(parents=True)
        except OSError:
            pass
        with open(str(file), encoding='latin1') as input:
            x = pipeline.named_steps['vgb'].transform(pipeline.named_steps['tokenizer'].transform([input.read()]))[0]
            with open(str(dst_file) + ".bow", 'w') as out:
                nums = [int(n) for n in x.split(' ')]
                map = {}
                for n in nums:
                    if n not in map:
                        map[n] = 0
                    map[n] += 1
                for key in map.keys():
                    out.write(str(key) + "\t" + str(map[key]) + "\t" + alpha[key] + "\n")


def compare_restored_score(X_tr, y_tr, X_te, y_te, size=15000, iter=15, filename=None, restore=False):
    vgb = VGramBuilderP(size=size, iter=iter)
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", vgb)
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
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(traint, y_tr)
    print("train accuracy: ", np.mean(pipeline.predict(traint) == y_tr))
    predicted = pipeline.predict(testt)
    print("test accuracy: ", np.mean(predicted == y_te))
    vgram_features.named_steps['vgb'].save(filename)

    rvgb = VGramBuilderP(filename=filename)
    rvgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", rvgb)
    ])
    rvgram_features.fit(data)
    traint = rvgram_features.transform(X_tr)
    testt = rvgram_features.transform(X_te)
    print("train accuracy: ", np.mean(pipeline.predict(traint) == y_tr))
    predicted = pipeline.predict(testt)
    print("test accuracy: ", np.mean(predicted == y_te))

    alpha1 = sorted(vgram_features.named_steps['vgb'].alpha())
    alpha2 = sorted(rvgram_features.named_steps['vgb'].alpha())
    if alpha1 == alpha2:
        print("alphas equal")
    else:
        if (len(alpha1) != len(alpha2)):
            print(len(alpha1), len(alpha2))
        for i in range(len(alpha1)):
            if alpha1[i] != alpha2[i]:
                print(alpha1[i], alpha2[i])

    # coef_interpretation(vgram_features, pipeline)


def coef_interpretation(vgram_features, pipeline):
    alpha = vgram_features.named_steps['vgb'].alpha()
    alpha = vgram_features.named_steps['tokenizer'].decode(alpha)
    # for word in alpha:
    #     print(word)
    coef = pipeline.named_steps['clf'].coef_['w']
    print(len(alpha), len(coef))
    alpha_coefs = zip(alpha, coef)
    alpha_coefs = sorted(alpha_coefs, key=lambda p: -p[1])
    for i in range(50):
        print(alpha_coefs[i])
    # for all_union
    # alpha = vgram_features.named_steps['vgb'].alpha()
    # alpha = vgram_features.named_steps['tokenizer'].decode(alpha)
    # words = list(fu.named_step['words'].vocabulary_)
    # coef = pipeline.named_steps['clf'].coef_
    # print(len(alpha), len(words), len(coef))
    # alpha_coefs = zip(alpha + words, coef)
    # alpha_coefs = sorted(alpha_coefs, key=lambda p: -p[1])
    # for i in range(50):
    #     print(alpha_coefs[i])


def only_words(X_tr, y_tr, X_te, y_te):
    pipeline = Pipeline([
        ("words", CountVectorizer()),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    print("train accuracy: ", np.mean(pipeline.predict(X_tr) == y_te))
    predicted = pipeline.predict(X_te)
    print("test accuracy: ", np.mean(predicted == y_te))
    # voc = pipeline.named_steps['words'].vocabulary_
    # print(type(voc), len(voc))
    # keys = list(voc.keys())
    # print(keys[0], voc[keys[0]])
    # print(keys[1], voc[keys[1]])


def only_vgram(X_tr, y_tr, X_te, y_te, vgb): #size=15000, iter=15, filename=None, restore=False):
    traint = vgb.transform(X_tr)
    testt = vgb.transform(X_te)
    pipeline = Pipeline([
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(traint, y_tr)
    print("train accuracy: ", np.mean(pipeline.predict(traint) == y_tr))
    predicted = pipeline.predict(testt)
    print("test accuracy: ", np.mean(predicted == y_te))
    # coef_interpretation(vgram_features, pipeline)


# дичь
def vgram_and_words(X_tr, y_tr, X_te, y_te, unsup=None, size=15000, iter=15):
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilderP(size, iter))
    ])
    data = [x for x in X_tr]
    for x in X_te:
        data.append(x)
    if unsup is not None:
        for x in unsup:
            data.append(x)
    vgram_features.fit(data)
    vX_tr = vgram_features.transform(X_tr)
    vX_te = vgram_features.transform(X_te)

    vgram_features_vec = CountVectorizer().fit(vX_tr)
    vcX_tr = vgram_features_vec.transform(vX_tr)
    vcX_te = vgram_features_vec.transform(vX_te)

    words_features = CountVectorizer().fit(X_tr)
    wX_tr = words_features.transform(X_tr)
    wX_te = words_features.transform(X_te)
    words_without_vgrams = set(words_features.vocabulary_)
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


def union_error(X_tr, y_tr, X_te, y_te, vgb):
    # fu = FeatureUnion([("vgram", vgb),
    #                    ("words", CountVectorizer())])
    # fu.fit(X_tr)
    # X_tr = fu.transform(X_tr)
    # X_te = fu.transform(X_te)

    X_tr_v = vgb.transform(X_tr)
    X_te_v = vgb.transform(X_te)
    words_vect = CountVectorizer()
    words_vect.fit(X_tr)
    X_tr_w = words_vect.transform(X_tr)
    X_te_w = words_vect.transform(X_te)
    print(X_tr_w.shape)
    print(X_tr_v.shape)
    X_tr = np.hstack((X_tr_v, X_tr_w))
    X_te = np.hstack((X_te_v, X_te_w))
    print(X_tr.shape)

    features_union = FeatureUnion([("vgram", vgb), ("words", CountVectorizer())])
    # X_tr = features_union.fit(X_tr).transform(X_tr)
    # X_te = features_union.transform(X_te)
    # print(X_tr.shape)
    # print(X_te.shape)
    pipeline = Pipeline([
        # ("features", features_union),
        # ("words", CountVectorizer()),
        # ("vgram", vgram_features),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    # pipeline = pipeline.fit(X_tr, y_tr)
    # print("train accuracy: ", np.mean(pipeline.predict(X_tr) == y_te))
    # predicted = pipeline.predict(X_te)
    # print("test accuracy: ", np.mean(predicted == y_te))


def all_union(X_tr, y_tr, X_te, y_te, vgb):# size=15000, iter=15, filename=None, restore=False):
    # fu = FeatureUnion([("vgram", vgb),
    #                    ("words", CountVectorizer())])
    # fu.fit(X_tr)
    # X_tr = fu.transform(X_tr)
    # X_te = fu.transform(X_te)

    # X_tr_v = vgb.transform(X_tr)
    # X_te_v = vgb.transform(X_te)
    # words_vect = CountVectorizer()
    # words_vect.fit(X_tr)
    # X_tr_w = words_vect.transform(X_tr)
    # X_te_w = words_vect.transform(X_te)
    # X_tr = np.hstack((X_tr_v, X_tr_w))
    # X_te = np.hstack((X_te_v, X_te_w))

    features_union = FeatureUnion([("vgram", vgb), ("words", CountVectorizer())])
    pipeline = Pipeline([
        ("features", features_union),
        # ("words", CountVectorizer()),
        # ("vgram", vgram_features),
        ('tfidf', TfidfTransformer()),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50, random_state=42))
    ])
    pipeline = pipeline.fit(X_tr, y_tr)
    print("train accuracy: ", np.mean(pipeline.predict(X_tr) == y_te))
    predicted = pipeline.predict(X_te)
    print("test accuracy: ", np.mean(predicted == y_te))

    # alpha = vgram_features.named_steps['vgb'].alpha()
    # alpha = vgram_features.named_steps['tokenizer'].decode(alpha)
    # words = list(fu.named_step['words'].vocabulary_)
    # coef = pipeline.named_steps['clf'].coef_
    # print(len(alpha), len(words), len(coef))
    # alpha_coefs = zip(alpha + words, coef)
    # alpha_coefs = sorted(alpha_coefs, key=lambda p: -p[1])
    # for i in range(50):
    #     print(alpha_coefs[i])


def vgram_to_file(root, vgb):
    start = time.time()
    write_vgram_bows_to_file(vgb, root + "train")
    write_vgram_bows_to_file(vgb, root + "test")
    print("time for writing bows: ", time.time() - start)


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
    print("test data shape:  ", len(X_te), len(X_te[0]), len(y_te))
    return X_tr, y_tr, X_te, y_te


def vgb_pipeline(args):
    if args.restore:
        vgb = VGramBuilderP(filename=args.filename)
    else:
        vgb = VGramBuilderP(size=int(args.size), iter=int(args.iter))
    vgram_features = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", vgb),
        ("vect", CountVectorizer())
    ])
    if args.dataset == "imdb":
        data = imdb_data("/Users/akhvorov/data/datasets/aclImdb/", unsup=True)
    else:
        print("add new dataset to vgb_pipeline")
    print("texts for vgram learning:", len(data))
    vgram_features.fit(data)
    if not args.restore:
        vgram_features.named_steps['vgb'].save(args.filename)
    return vgram_features


def apply_model(X_tr, y_tr, X_te, y_te, args):
    print("mode: ", end='')
    if args.mode == "v":
        print("only vgram")
        vgb = vgb_pipeline(args)
        only_vgram(X_tr, y_tr, X_te, y_te, vgb) #, int(args.size), int(args.iter), dict_name, args.restore)
    elif args.mode == "w":
        print("only words")
        only_words(X_tr, y_tr, X_te, y_te)
    elif args.mode == "w_or_v" or args.mode == "v_or_w":
        print("w|v, union of two feature sets")
        vgb = vgb_pipeline(args)
        all_union(X_tr, y_tr, X_te, y_te, vgb)# int(args.size), int(args.iter), dict_name, args.restore)
    elif args.mode == "w_and_v" or args.mode == "v_and_w":
        print("w&v, two features sets without intersection. it works not so good")
        vgram_and_words(X_tr, y_tr, X_te, y_te, int(args.size), int(args.iter))
    else:
        print("wrong mode")
    # test_20ng()
    # restore_test()
    # all_union(X_tr, y_tr, X_te, y_te)
    # only_vgram(X_tr, y_tr, X_te, y_te)
    # vgram_and_words(X_tr, y_tr, X_te, y_te)


def arg_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dataset')
    parser.add_argument('-m', '--mode')
    parser.add_argument('-s', '--size', default=15000)
    parser.add_argument('-i', '--iter', default=15)
    parser.add_argument('-r', '--restore', action='store_true')
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
    dict_name = "/Users/akhvorov/data/datasets/aclImdb3/" + args.dataset + "_" + args.size + "_" + args.iter + ".cpp.dict"
    # dict_name = "/Users/akhvorov/data/datasets/aclImdb2/dict_200.cpp.dict"
    args.filename = dict_name
    print("dict path:", args.filename)
    start = time.time()

    X_tr, y_tr, X_te, y_te = get_data(args.dataset)
    # translate_dict(X_tr, X_te, "/Users/akhvorov/data/datasets/aclImdb2/dict_15000.dict", "/Users/akhvorov/data/datasets/aclImdb2/dict_15000.cpp.dict", "java")
    # translate_dict(X_tr, X_te, "/Users/akhvorov/data/datasets/aclImdb2/dict_15000.cpp.dict", "/Users/akhvorov/data/datasets/aclImdb2/dict_15000.java.dict", "cpp")
    # vgb = vgb_pipeline(args)
    # translate_dict("/Users/akhvorov/data/datasets/aclImdb/", dict_name, "/Users/akhvorov/data/datasets/aclImdb3/imdb_15000_3.java.dict", "cpp")
    # print("dict translated")
    # apply_model(X_tr, y_tr, X_te, y_te, args)
    union_error(X_tr, y_tr, X_te, y_te, vgb_pipeline(args))
    # vgram_to_file("/Users/akhvorov/data/datasets/aclImdb3/", vgb)
    print("time: ", time.time() - start)


if __name__ == "__main__":
    main()


'''
without_split -> fit_ws -> 

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
