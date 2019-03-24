import numpy as np
from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer
from sklearn.linear_model import SGDClassifier
from sklearn.pipeline import Pipeline
from vgram import loadVGram, VGram
from binding.src.main.python.examples.data_reader import get_20ng, get_imdb


def learn_svm_on_vgram(X_tr, X_te, y_tr, y_te):
    vgram = Pipeline([
        ("vgb", VGram(15000, 10)),
        ("vect", CountVectorizer())
    ])
    vgram.fit(X_tr + X_te)

    pipeline = Pipeline([
        ("features", vgram),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100))
    ])
    pipeline.fit(X_tr, y_tr)

    print(f"train accuracy: {np.mean(pipeline.predict(X_tr) == y_tr)}")
    print(f"test accuracy: {np.mean(pipeline.predict(X_te) == y_te)}")


# Then we use small dictionary, less data and less iteration only for faster fitting.
# This is bad parameters for real-world solution, see learn_svm_on_vgram for good parameters example


def save_learned_vgram(X_tr, X_te, path):
    data = X_tr + X_te
    data = data[:1000]
    data = [s.encode('utf-8') for s in data]
    vgram = VGram(500, 1)
    vgram.fit(data)
    vgram.save(path)


def load_learned_vgram_for_svm(X_tr, X_te, y_tr, y_te, path):
    pipeline = Pipeline([
        ("vgb", loadVGram(path)),
        ("vectorizer", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100))
    ])
    pipeline.fit(X_tr, y_tr)
    print(f"train accuracy: {np.mean(pipeline.predict(X_tr) == y_tr)}")
    print(f"test accuracy: {np.mean(pipeline.predict(X_te) == y_te)}")


def show_learned_dictionary(path):
    vgram = loadVGram(path)
    print(vgram.alphabet()[:10])


if __name__ == "__main__":
    path = "vgram_dict.json"
    X_tr, X_te, y_tr, y_te = get_20ng()
    # X_tr, X_te, y_tr, y_te = get_imdb()
    # learn_svm_on_vgram(X_tr, X_te, y_tr, y_te)
    # save_learned_vgram(X_tr, X_te, path)
    # show_learned_dictionary(path)
    load_learned_vgram_for_svm(X_tr, X_te, y_tr, y_te, path)
