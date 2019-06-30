
.. _examples:

Examples
########

Basic example
=============

This function accept list of strings, fit v-gram dictionary and transform to v-gram representation.

.. code-block:: python

    from vgram import VGram

    def split_text_on_vgram(tests):
        vgram = VGram(10000, 10)
        vgram.fit(texts)
        return vgram.transform(texts)

Save dictionary
===============

Save fitted dictionary to file and restore them

.. code-block:: python

    from vgram import VGram, loadVGram

    vgram = VGram(5000, 10)
    vgram.fit(texts)
    vgram.save("my_dict.json")
    vgram = loadVGram("my_dict.json")

Include in scikit-learn pipeline
================================

VGram can be embedded to sklearn pipeline as text preprocessing.

.. code-block:: python

    from sklearn.feature_extraction.text import CountVectorizer, TfidfTransformer\
    from sklearn.linear_model import SGDClassifier
    from sklearn.pipeline import Pipeline
    from vgram import VGram, loadVGram

    def classification_on_vgrams(texts, labels):
        pipeline = Pipeline([
            ("vgram", VGram(10000, 10)),
            ("vect", CountVectorizer())
            ('tfidf', TfidfTransformer()),
            ('clf', SGDClassifier())
        ])
        pipeline.fit(texts, labels)
        print("train accuracy: ", np.mean(pipeline.predict(texts) == labels))

Real example
============

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
Once fitted, v-gram doesn't fit again, and we could not trouble about double fitting.

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

Build v-grams on int sequences
==============================

IntVGram helps you to work with int sequences.
Standard fit-transform return list of strings for usage in sklearn text pipeline (see `IntVGram in text pipeline`_).
Apply `split()` to make integer lists.

.. code-block:: python

	from vgram import IntVGram

    int_seqs = [[1, 1, 1, 2], [1, 2, 1, 2]] # 2-d int list or numpy array
	vgram = IntVGram(3, 1000)
	vgram.fit(int_seqs)
	text_vgrams = vgram.transform(int_seqs) # strings like "0 1 0 1"
	int_vgrams = [s.split() for s in text_vgrams] # [[0, 0, 0, 1], [0, 1, 0, 1]]

IntVGram in text pipeline
=========================

This example is equivalent to usage of VGram but uses integer streams instead strings.

.. code-block:: python

	from sklearn.pipeline import Pipeline, FeatureUnion
	from vgram import IntVGram

	pipeline = Pipeline([
		("vgram", IntVGram(10000, 10)),
		("vect", CountVectorizer()),
		('tfidf', TfidfTransformer(sublinear_tf=True)),
		('clf', SGDClassifier(loss='hinge', penalty='l2', alpha=1e-4, max_iter=100))
	])
	pipeline.fit(int_seqs, labels)

Save and load v-grams
=====================

Yo can fit any v-gram dictionary and save to file. Then it can be restored by load methods.

.. code-block:: python

    from vgram import VGram, loadVGram

    vgram = VGram(5000, 10)
    vgram.fit(data)
    vgram.save("my_dict.json")
    vgram2 = loadVGram("my_dict.json")
    vgram2.transform(data)
    alphabet = vgram2.alphabet()

Construct VGram from file
=========================

.. code-block:: python

    vgram = loadVGram("/path/to/file")
    vgram_pipeline = Pipeline([
        ("vgb", vgram),
        ("vect", CountVectorizer())
    ])
    vgram_pipeline.fit(data)

.. Note::

    VGram fit only once and wouldn't be fitted again. Only CountVectorizer will be fitted.

Saving intermediate dictionaries to file
========================================

.. code-block:: python

    vgram = Pipeline([
        ("tokenizer", CharTokenizer()),
        ("vgb", VGramBuilder(10000, 10, "/path/to/file")),
        ("vect", CountVectorizer())
    ])
    vgram.fit(data)

StreamVGram
===========

.. code-block:: python

    from vgram import StreamVGram

    vgram = StreamVGram(5000)
    for seq in seqs:  # some stream of sequences, maybe infinite
        vgram.accept(seq)
    vgram.update()  # don't forget it!
    parsed_seq = vgram.parse(seq)

Load StreamVGram from file
==========================

Let's read an existing dictionary from the file, fit it more and save.
If you have little data, you can train a dictionary on a large dataset (e.g., all wikipedia articles) and save it.
Then fit more on domain-specific data for your task and get a better result than if you fit only on this data.

.. code-block:: python

    import random
    from vgram import loadStreamVGram

    vgram = loadStreamVGram("common_dict.json")
    n_times = 10
    for iters in range(n_times):  # feed data to the model few times until convergence
        for i in range(len(little_data)):
            vgram.accept(little_data[random.randint(0, len(little_data) - 1])
    vgram.update()
    parsed_seq = vgram.parse(seq)
    vgram.save("task_specific_dict.json")

Fine-tune StreamVGram
=====================

You can pre-train VGram, save it, load as StreamVGram and fine-tune.
Unfortunately, to do the opposite will not work.

.. code-block:: python

    import random
    from vgram import VGram, StreamVGram, loadStreamVGram

    vgram = VGram(5000, 10)
    vgram.fit(data)
    vgram.save("dict.json")
    stream_vgram = loadStreamVGram("dict.json")
    n_times = 10
    for iters in range(n_times):  # feed data to the model few times until convergence
        for i in range(len(little_data)):
            stream_vgram.accept(little_data[random.randint(0, len(little_data) - 1])
    stream_vgram.update()
    parsed_seq = vgram.parse(seq)
    stream_vgram.save("task_specific_dict.json")


Our experiments
===============

You can find our experiments `there <https://github.com/akhvorov/vgram/blob/master/binding/src/main/python/synthetic_tests.py>`_.
