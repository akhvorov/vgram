
.. _builder:

V-Gram building
###############

StreamVGramBuilder
==================

Use this class for construction v-gram dictionary in common cases.
It have universal interface for any sequences performed by list of integer numbers.
This can be useful in cases where data does not fit into memory or coming by stream.
Every sequence accepted separately and fit dictionary gradually.

If it was a class in Python, it would look like this:

.. code-block:: python

    class StreamVGramBuilder:
        def __init__(self, size): .. # filename="", verbose=1
        def __init__(self, size, filename): .. # verbose=1
        def __init__(self, size, verbose): .. # filename=""
        def __init__(self, size, filename, verbose): ..
        def __init__(self, filename): ..
        def __init__(self, filename, verbose=1): .. # verbose=1
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename="", tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..


:class:`StreamVGramBuilder` have two constructors. One for building new dictionary and second for using existing one.
For using second constructor you should already build dictionary before and saved it.
It's recommended way to use v-grams because dictionary building takes a long time.

| ``StreamVGramBuilder(size, filename="", verbose=1)`` building new dictionary of size ``size``.
| Pass ``filename`` for dictionary saving dictionary by method ``save``.
| Also you can do ``save()`` without filename argument, this ``filename`` will be used.
| ``verbose`` means level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``StreamVGramBuilder(filename, verbose=1)`` building dictionary from file. It works if you fit dictionary and save to file before.

``accept(seq)`` consume 1-d list of integer or 1-d numpy array.
For fitting dictionary make ``accept`` of data items enough times. If ``verbose=1`` some information will be printed.
If the ``compression ratio`` has ceased to vary greatly means the dictionary has learned.

``parse(X)`` consume 1-d list of integer or 1-d numpy array.
Return 1-d list of integers, where every integer is v-gram index in the alphabet.

``update()`` it's very important method for using v-grams dictionary.
Before using dictionary make ``update()``. It update dictionary and make ready for using for parse data or anything else.

``save(filename, tokenizer=None)`` consume filename where dict will be save and optionally tokenizer for more interpretable dictionary file.
See subsection below about the save format of the dictionary.

``alphabet()`` return 2-d list of integers, where every list of integers is builded v-grams.

``freqs()`` return 1-d list of integers with frequencies of v-gram occurrence in data.

See :ref:`examples` for details.

VGramBuilder
============

:class:`VGramBuilder` is a child class of :class:`StreamVGramBuilder`.
For working with v-grams in simple cases use ``VGramBuilder``.
This class implement scikit-learn fit-transform interface and can be well embedded in existing code.

If it was a class in Python, it would look like this:

.. code-block:: python

    class VGramBuilder(StreamVGramBuilder):
        def __init__(self, size, iters): .. # filename="", verbose=1
        def __init__(self, size, iters, filename): .. # verbose=1
        def __init__(self, size, iters, verbose): .. # filename=""
        def __init__(self, size, iters, filename, verbose): ..
        def __init__(self, filename): .. # verbose=1
        def __init__(self, filename, verbose): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename="", tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..

Constructors is the same as :class:`StreamVGramBuilder` but ``iters`` is added because dictionary learned only once by passing data to algorithm ``iters`` time.
If dictionary construct from file, it's already fitted and don't need ``iters``.
Same as in ``StreamVGramBuilder`` for using second constructor you should already build dictionary before and saved it.
It's recommended way to use v-grams because dictionary building takes a long time.

| ``VGramBuilder(size, iters, filename="", verbose=1)`` building new dictionary of size ``size`` and learn ``iters`` iterations.
| Pass ``filename`` for dictionary saving after each iteration. It may be useful, if you start building dictionary with more iterations as needed and want to interrupt process without loosing progress. Feel free to do so, if ``filename`` is provided, dictionary will be saved.
| Also you can do ``save()`` without filename argument, this ``filename`` will be used.
| ``verbose`` means level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``VGramBuilder(filename, verbose=1)`` building dictionary from file. It works if you fit dictionary and save to file before.

``fit(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Make ``iters`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Return 1-d list of strings, where each string is integers joined by space, where every integer is v-gram index in the alphabet.
It's good for pipeline where CountVectorizer follows VGramBuilder (see :ref:`examples`).

``accept(seq)``, ``parse(X)`` and ``update()`` works same as in :class:`StreamVGramBuilder`.
You can fit dictionary by ``fit()`` and continue fitting by ``accept``.
It's not recommended way byt may be useful in specific cases.

``save(filename="", tokenizer=None)`` consume filename where dict will be save and optionally tokenizer for more interpretable dictionary file.
If ``filename`` not passed used ``filename`` from constructor.

``alphabet()`` return 2-d list of integers, where every list of integers is builded v-grams.

``freqs()`` return 1-d list of integers with frequencies of v-gram occurrence in data.

If you work with integers streams, VGramBuilder is enough, but for text conveniently to use tokenizers (see :ref:`tokenizers`).

See :ref:`examples` for details.

Saved dictionary format
=======================

Dictionary saved as json-formatted file:

.. code-block:: json

    {
      "alphabet": [
        {
          "freq": 1188,
          "text": "fromthe",
          "vec": [
            0, 1, 2, 3, 15, 8, 6
          ]
        },
        ..
      ],
      "coder": [0, 1, 2, 3, 18, 12, ..],
      "size": 1000,
      "min_prob": 3.7657904299967802e-06,
      "fitted": true,
      "freqs_computed": true,
    }

If ``tokenizer`` has passed to ``save`` method field ``text`` contains text of v-gram in ``alphabet`` items.
Without ``tokenizer`` the text field will not be in the file.

After v-grams construction you can analyse the resulting dictionary.
``alphabet`` is a list of v-gram objects

``freq`` is a frequencies of v-gram occurrence in data.
``vec`` is a vector of language alphabet symbols for v-gram presentation.

``coder`` is a sequence of symbols as they occur in the data.
``size`` is a size of dictionary
``min_prob``, ``fitted`` and ``freqs_computed`` are inner information for model.
``fitted`` and ``freqs_computed`` provided only for VGramBuilder class.
