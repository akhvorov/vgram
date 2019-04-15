
.. _examples:

Examples
########

Basic example
=============

The primary example of 20 newsgroups dataset classification

.. code-block:: python

	import numpy as np
	from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer
	from sklearn.linear_model import SGDClassifier
	from sklearn.datasets import fetch_20newsgroups
	from sklearn.pipeline import Pipeline
	from vgram import VGram

	# fetch data
	train, test = fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
	data = train.data + test.data

	# make vgram pipeline and fit it
	vgram = VGram(10000, 10)
	# it's ok, vgb fit only once
	vgram.fit(data)

	# fit classifier and get score
	pipeline = Pipeline([
		("features", vgram),
		("vect", CountVectorizer())
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=1002))
	])
	pipeline.fit(train.data, train.target)
	print("train accuracy: ", np.mean(pipeline.predict(train.data) == train.target))
	print("test accuracy: ", np.mean(pipeline.predict(test.data) == test.target))

	# show first ten elements of constructed vgram dictionary
	alpha = vgram.alphabet()
	print("First 10 alphabet elements:", alpha[:10])

V-Gram is an unsupervised method that's why we fit v-gram to all data.
Once fitted, v-gram doesn't fit again, and we could not trouble about doubled fitting.

In the last two lines shown how to get dictionary alphabet and print some elements.

Words and v-grams union
=======================

We just make the union of features.

.. code-block:: python

	from sklearn.pipeline import Pipeline, FeatureUnion

	vgram = Pipeline([
		("vgb", VGram(10000, 10)),
		("vect", CountVectorizer())
	])
	vgram.fit(data)

	pipeline = Pipeline([
		("features", FeatureUnion([("vgb", vgram), ("words", CountVectorizer())])),
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100))
	])

It's easy to improve your existing project by adding v-grams.

Build vgrams on int sequences
=============================

IntVGram helps you to work with int seqs. If default tokenizer is not good for your task,
you can create your own (see below) and use it.
This example show is equivalent to usage of VGram cause of CharTokenizer is default tokenizer for VGram.

.. code-block:: python

	from sklearn.pipeline import Pipeline, FeatureUnion
	from vgram import IntVGram, CharTokenizer

	vgram = Pipeline([
	    ("tokenizer", CharTokenizer()),
		("vgb", IntVGram(10000, 10))
	])
	vgram.fit(data)

	pipeline = Pipeline([
		("vgram", vgram),
		("vect", CountVectorizer()),
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100))
	])


Custom Tokenizer
================

You should override only two methods in BaseTokenizer for creation custom tokenizer.

.. code-block:: python

    from vgram import VGramBuilder, BaseTokenizer

    class WordTokenizer(BaseTokenizer):
        def normalize(self, X):
            return [re.sub("[^ \w\d]", "", re.sub(" +", " ", x)).lower() for x in X]

        def tokenize(self, X):
            return [x.split(" ") for x in X]

    vgram = Pipeline([
        ("tokenizer", WordTokenizer()),
        ("vgb", VGramBuilder(10000, 10)),
        ("vect", CountVectorizer())
    ])

More Custom Tokenizer
=====================

If BaseTokenizer doesn't fit your task, you can create your own tokenizer.

.. code-block:: python

    class Tokenizer:
        def __init__(self):
            self.forward = {}
            self.backward = {}

        def fit(self, X):
            return self

        def transform(self, X):
            res = []
            for x in X:
                s = []
                for c in x.lower():
                    if c.isalnum():
                        if c not in self.forward:
                            self.forward[c] = len(self.forward)
                            self.backward[self.forward[c]] = c
                        s.append(self.forward[c])
                res.append(s)
            return res

        def decode(self, X):
            res = []
            for x in X:
                res.append([self.backward[c] for c in x if c in self.backward])
            return res

        def fit_transform(self, X):
            return self.fit(X).transform(X)

    tok = Tokenizer()
    tok_texts = tok.fit_transform(texts)
    vgram = IntVGram(10000, 10)
    vgram.fit(tok_texts)
    print("Alphabet:", tok.decode(vgram.alphabet())

Save VGram to file
==================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", IntVGram(10000, 10, 0)),  # verbose=0
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)
    vgram.named_steps["vgb"].save("/path/to/file")  # saving without readable words
    # vgram.named_steps["vgb"].save("/path/to/file", vgram.named_steps["tokenizer"])  # saving with readable words

Construct VGramBuilder from file
================================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilder("/path/to/file")),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

.. Note::

    VGramBuilder fit only once and wouldn't be fitted again. Only CharTokenizer and CountVectorizer will be fitted.

Saving intermediate dictionaries to file
========================================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilder(10000, 10, "/path/to/file")),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

StreamVGramBuilder
==================

.. code-block:: python

    from vgram import StreamVGramBuilder

    vgram = StreamVGramBuilder(5000)
    for seq in seqs:  # some stream of sequences, maybe infinite
        vgram.accept(seq)
    vgram.update()  # don't forget it!
    parsed_seq = vgram.parse(seq)

StreamVGramBuilder from file
============================

Let's read an existing dictionary from the file, fit it more and save.
If you have little data, you can train a dictionary on a large dataset (e.g., all wikipedia articles) and save it.
Then fit more on domain-specific data for your task and get a better result than if you fit only on this data.

.. code-block:: python

    import random
    from vgram import StreamVGramBuilder

    vgram = StreamVGramBuilder("common_dict.json", 1)  # verbose=1
    n_times = 10
    for iters in range(n_times):  # feed data to the model few times until convergence
        for i in range(len(little_data)):
            vgram.accept(little_data[random.randint(0, len(little_data) - 1])
    vgram.update()
    parsed_seq = vgram.parse(seq)
    vgram.save("task_specific_dict.json")


Our experiments
===============

You can find our experiments `there <https://github.com/akhvorov/vgram/blob/master/binding/src/main/python/synthetic_tests.py>`_.
