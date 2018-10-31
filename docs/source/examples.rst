
.. _examples:

Examples
########

Basic example
=============

Basic example of 20 news groups dataset classification

.. code-block:: python

	import numpy as np
	from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer
	from sklearn.linear_model import SGDClassifier
	from sklearn.datasets import fetch_20newsgroups
	from sklearn.pipeline import Pipeline
	from vgram import VGramBuilder, CharTokenizer

	# fetch data
	train, test = fetch_20newsgroups(subset='train'), fetch_20newsgroups(subset='test')
	data = train.data + test.data

	# make vgram pipeline and fit it
	vgram = Pipeline([
		("tokenizer", CharTokenizer()),
		("vgb", VGramBuilder(10000, 10)),
		("vect", CountVectorizer())
	])
	# it's ok, vgram fit only once
	vgram.fit(data)

	# fit classifier and get score
	pipeline = Pipeline([
		("features", vgram),
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=42))
	])
	pipeline.fit(train.data, train.target)
	print("train accuracy: ", np.mean(pipeline.predict(train.data) == train.target))
	print("test accuracy: ", np.mean(pipeline.predict(test.data) == test.target))

	# show first ten elements of constructed vgram dictionary
	alpha = vgram.named_steps["tokenizer"].decode(vgram.named_steps["vgb"].alphabet())
	print("First 10 alphabet elements:", alpha[:10])

V-Gram is unsupervised method that's why we fit vgram to all data.
Once fitted, vgram don't fit again and we could not trouble about doubled fitting.

In last two lines shown how get dictionary alphabet and print some elements.

Words and vgrams union
======================

We simply make union of features

.. code-block:: python

	from sklearn.pipeline import Pipeline, FeatureUnion

	vgram = Pipeline([
		("tokenizer", CharTokenizer()),
		("vgb", VGramBuilder(10000, 10)),
		("vect", CountVectorizer())
	])
	vgram.fit(data)

	pipeline = Pipeline([
		("features", FeatureUnion([("vgb", vgram), ("words", CountVectorizer())])),
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100, random_state=42))
	])

Custom Tokenizer
================

.. code-block:: python

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

Save VGramBuilder to file
=========================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilder(10000, 10)),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)
    vgram.named_steps["vgb"].save("/path/to/file")

Make VGramBuilder from file
===========================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilder("/path/to/file")),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

.. Note::

    VGramBuilder fit only once and wouldn't be fitted again. Only CharTokenizer and CountVectorizer will be fitted.

More examples
=============

More examples you can find `there <https://github.com/akhvorov/vgram/blob/master/binding/src/main/python/synthetic_tests.py>`_.
