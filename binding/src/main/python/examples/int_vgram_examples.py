import numpy as np
from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer
from sklearn.linear_model import SGDClassifier
from sklearn.pipeline import Pipeline
from vgram import IntVGram, loadVGram, loadIntVGram, CharTokenizer
from .data_reader import get_20ng, get_imdb


def learn_svm_on_int_vgram(X_tr, X_te, y_tr, y_te):
    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", IntVGram(15000, 10)),
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

    alpha = vgram.named_steps["tokenizer"].decode(vgram.named_steps["vgb"].alphabet())
    print("First 10 alphabet elements:", alpha[:10])


def save_learned_int_vgram(X_tr, X_te, path):
    data = X_tr + X_te
    data = data[:1000]
    vgram_pipeline = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgram", IntVGram(500, 1))
    ])
    vgram_pipeline.fit(data)
    vgram_pipeline.named_steps["vgram"].save(path)


def save_learned_int_vgram_to_vgram(X_tr, X_te, path):
    data = X_tr + X_te
    data = data[:1000]
    vgram_pipeline = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgram", IntVGram(500, 1))
    ])
    vgram_pipeline.fit(data)
    vgram_pipeline.named_steps["vgram"].save(path, vgram_pipeline.named_steps["tokenizer"])


def load_learned_int_vgram_for_svm(X_tr, X_te, y_tr, y_te, path):
    pipeline = Pipeline([
        ("tokenizer", CharTokenizer()),  # tokenizer should fit on same data as previous tokenizer
        ("vgb", loadIntVGram(path)),
        ("vectorizer", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50))
    ])
    pipeline.fit(X_tr, y_tr)
    print(f"train accuracy: {np.mean(pipeline.predict(X_tr) == y_tr)}")
    print(f"test accuracy: {np.mean(pipeline.predict(X_te) == y_te)}")


def load_learned_vgram_for_svm(X_tr, X_te, y_tr, y_te, path):
    pipeline = Pipeline([
        ("vgb", loadVGram(path)),
        ("vectorizer", CountVectorizer()),
        ('tfidf', TfidfTransformer(sublinear_tf=True)),
        ('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=50))
    ])
    pipeline.fit(X_tr, y_tr)
    print(f"train accuracy: {np.mean(pipeline.predict(X_tr) == y_tr)}")
    print(f"test accuracy: {np.mean(pipeline.predict(X_te) == y_te)}")


def show_learned_dictionary(X_tr, X_te, path):
    data = X_tr + X_te
    tokenizer = CharTokenizer()
    tokenizer.fit(data)  # don't works, use next line
    tokenizer.transform(data)
    vgram = loadIntVGram(path)
    print(tokenizer.decode(vgram.alpabet()[:20]))


if __name__ == "__main__":
    int_vgram_dict = "int_vgram_dict.json"
    vgram_dict = "int_vgram_dict.json"
    X_tr, X_te, y_tr, y_te = get_20ng()

    save_learned_int_vgram(X_tr, X_te, int_vgram_dict)
    load_learned_int_vgram_for_svm(X_tr, X_te, y_tr, y_te, int_vgram_dict)

    save_learned_int_vgram_to_vgram(X_tr, X_te, vgram_dict)
    load_learned_vgram_for_svm(X_tr, X_te, y_tr, y_te, vgram_dict)
