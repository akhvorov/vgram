
.. _builder:

V-Gram building
###############

VGramBuilder
============

For working with v-grams in simple cases use ``VGramBuilder``.
This class implement scikit-learn fit-transform interface and can be well embedded in existing code (see examples).

If it was a class in Python, it would look like this:

.. code-block:: python

    class VGramBuilder(StreamVGramBuilder):
        def __init__(self, size, iters, filename="", verbose=1): ..
        # yes, two different constructors
        def __init__(self, filename, verbose=1): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def save(self, filename, tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..

:class:`VGramBuilder` have two constructors. One for building new dictionary and second for using existing dictionary.
For using second constructor you should already build dictionary before and saved it.
It's recommended way to use v-grams because dictionary building takes a long time.

| ``VGramBuilder(size, iters, filename, verbose)`` building new dictionary of size ``size`` and learn ``iters`` iterations.
| Pass ``filename`` for dictionary saving after each iteration. It may be useful, if you start building dictionary with more iterations as needed and want to interrupt process without loosing progress. Feel free to do so, if ``filename`` is provided, dictionary will be saved.
| Also you can do ``save()`` without filename argument, this ``filename`` will be used.
| ``verbose`` means level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``VGramBuilder(filename, verbose)`` building dictionary from file. It works if you fit dictionary and save to file before.

``fit(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Make ``iters`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Return 1-d list of strings, where each string is integers joined by space, where every integer is v-gram index in the alphabet.
It's good for pipeline where CountVectorizer follows VGramBuilder (see :ref:`examples`).

``save(filename, tokenizer=None)`` consume filename where dict will be save and optionally tokenizer for more interpretable dictionary file.

``alphabet()`` return 2-d list of integers, where every list of integers is builded v-grams.

``freqs()`` return 1-d list of integers with frequencies of v-gram occurrence in data.

If you work with integers streams, VGramBuilder is enough, but for text conveniently to use tokenizers (see :ref:`tokenizers`).

See :ref:`examples` for details.

StreamVGramBuilder
==================

``StreamVGramBuilder`` is a base class of ``VGramBuilder``.
This can be useful in cases where data does not fit into memory.
Every sequence accepted separately and fit dictionary gradually.

If it was a class in Python, it would look like this:

.. code-block:: python

    class StreamVGramBuilder:
        def __init__(self, size, filename="", verbose=1): ..
        def __init__(self, filename, verbose=1): ..
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename, tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..

Constructors is the same as :class:`VGramBuilder` but ``iters`` is removed because it always ready to fit dictionary by accept.

:class:`StreamVGramBuilder` is similar with VGramBuilder but three methods.

``accept(seq)`` consume 1-d list of integer or 1-d numpy array.
For fitting dictionary make ``accept`` of data items enough times.

``parse(X)`` consume 1-d list of integer or 1-d numpy array.
Return 1-d list of integers, where every integer is v-gram index in the alphabet.

``update()`` it's very important method for using v-grams dictionary.
Before using dictionary make ``update()``. It update dictionary and make ready for using for parse data or anything else.

:class:`StreamVGramBuilder` makes for working with integers only and not adjust for scikit-learn interfaces.

See :ref:`examples` for details.