
.. _builder:

V-Gram construction
###################

StreamVGram
===========

Use this class for construction v-gram dictionary in cases where data is too large or comes as stream.
This allows you to build with ``vgram`` more flexibly.
Every sequence accepted separately and fit dictionary gradually.

.. Note::
    Remember to call ``update`` method before parsing.

.. code-block:: python

    class StreamVGram:
        def __init__(self, size=10000, tokenizer=CharTokenizer(), verbose=0):
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename=""): ..
        def alphabet(self): ..
        def freqs(self): ..


:class:`StreamVGram` have two constructors. One for building a new dictionary and second for using an existing one.

| ``StreamVGram(size=10000, tokenizer=CharTokenizer(), verbose=0)`` building new dictionary of size ``size``.
| By default CharTokenizer is used. Now passing custom tokenizer is not stable, please use IntStreamVGram with this tokenizer.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``accept(seq)`` consume a 1-d list of integer or 1-d numpy array.
For fitting dictionary make ``accept`` of data items enough times. If ``verbose=1`` some information will be printed.
If the ``compression ratio`` has ceased to vary greatly means the dictionary has learned.

``parse(X)`` consume a 1-d list of integer or 1-d numpy array.
Return 1-d list of integers, where every integer is v-gram index in the alphabet.

``update()`` it's a very important method for using v-grams dictionary.
Before using dictionary make ``update()``. It updates dictionary and makes ready for using for parse data or anything else.

``save(filename)`` consume filename where dictionary will be saved.
See subsection below about the save format of the dictionary.

``alphabet()`` return a 2-d list of integers, where every list of integers is built v-grams.

``freqs()`` return a 1-d list of integers with frequencies of v-gram occurrence in data.

See :ref:`examples` for details.

VGram
=====

:class:`VGram` is a child class of :class:`StreamVGram`.
For working with v-grams in simple cases use ``VGram``.
This class implement the scikit-learn fit-transform interface and can be well embedded in the existing code.

.. code-block:: python

    class VGram(StreamVGram):
        def __init__(self, size=10000, iter_num=10, tokenizer=CharTokenizer(), verbose=0): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename=""): ..
        def alphabet(self): ..
        def freqs(self): ..

Constructors are the same as :class:`StreamVGram` but ``iter_num`` is added because dictionary learned only once by passing data to algorithm ``iter_num`` time.

| ``VGram(size=10000, iter_num=10, tokenizer=CharTokenizer(), verbose=0)`` building new dictionary of size ``size`` and learn ``iter_num`` iterations.
| By default CharTokenizer is used. Now passing custom tokenizer is not stable, please use IntStreamVGram with this tokenizer.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``fit(X)`` consume a 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Make ``iter_num`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume a 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Return a 1-d list of strings, where each string is integers joined by space, where every integer is v-gram index in the alphabet.
It's good for pipeline where CountVectorizer follows VGramBuilder (see :ref:`examples`).

``accept(seq)``, ``parse(X)`` and ``update()`` works same as in :class:`StreamVGram`.
You can fit dictionary by ``fit()`` and continue fitting by ``accept``.
It's not recommended way but may be useful in specific cases.

``save(filename="", tokenizer=None)`` consume filename where dictionary will be saved.

``alphabet()`` return a 2-d list of integers, where every list of integers is built v-grams.

``freqs()`` return a 1-d list of integers with frequencies of v-gram occurrence in data.

If you work with integers streams, VGramBuilder is enough, but for text conveniently to use tokenizers (see :ref:`tokenizers`).

See :ref:`examples` for details.

IntStreamVGram
==============

This class is similar to :class:`StreamVGram`, but works with integer numbers.
It's useful for working with time series.
If you want to make custom tokenization or other text normalization you can transform text to integer sequence and build vgrams on it.
It's more general way to use vgrams.

.. code-block:: python

    class IntStreamVGram:
        def __init__(self, size=10000, verbose=0): ..
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename="", tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..


:class:`StreamVGram` have two constructors. One for building a new dictionary and second for using an existing one.

| ``StreamVGram(size=10000, tokenizer=CharTokenizer(), verbose=0)`` building new dictionary of size ``size``.
| By default CharTokenizer is used. Now passing custom tokenizer is not stable, please use IntStreamVGram with this tokenizer.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``accept(seq)`` consume a 1-d list of integer or 1-d numpy array.
For fitting dictionary make ``accept`` of data items enough times. If ``verbose=1`` some information will be printed.
If the ``compression ratio`` has ceased to vary greatly means the dictionary has learned.

``parse(X)`` consume a 1-d list of integer or 1-d numpy array.
Return 1-d list of integers, where every integer is v-gram index in the alphabet.

``update()`` it's a very important method for using v-grams dictionary.
Before using dictionary make ``update()``. It updates dictionary and makes ready for using for parse data or anything else.

``save(filename, tokenizer=None)`` consume filename where dictionary will be saved and optionally tokenizer for more interpretable dictionary file.
See subsection below about the save format of the dictionary.

``alphabet()`` return a 2-d list of integers, where every list of integers is built v-grams.

``freqs()`` return a 1-d list of integers with frequencies of v-gram occurrence in data.

See :ref:`examples` for details.

VGram
=====

:class:`VGram` is a child class of :class:`StreamVGram`.
For working with v-grams in simple cases use ``VGram``.
This class implement the scikit-learn fit-transform interface and can be well embedded in the existing code.

.. code-block:: python

    class VGram(StreamVGram):
        def __init__(self, size=10000, iter_num=10, tokenizer=CharTokenizer(), verbose=0):
        def fit(self, X): ..
        def transform(self, X): ..
        def accept(self, seq): ..
        def parse(self, seq): ..
        def update(self): ..
        def save(self, filename="", tokenizer=None): ..
        def alphabet(self): ..
        def freqs(self): ..

Constructors are the same as :class:`StreamVGram` but ``iter_num`` is added because dictionary learned only once by passing data to algorithm ``iter_num`` time.

| ``VGram(size=10000, iter_num=10, tokenizer=CharTokenizer(), verbose=0)`` building new dictionary of size ``size`` and learn ``iter_num`` iterations.
| By default CharTokenizer is used. Now passing custom tokenizer is not stable, please use IntStreamVGram with this tokenizer.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``fit(X)`` consume a 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Make ``iter_num`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume a 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Return a 1-d list of strings, where each string is integers joined by space, where every integer is v-gram index in the alphabet.
It's good for pipeline where CountVectorizer follows VGramBuilder (see :ref:`examples`).

``accept(seq)``, ``parse(X)`` and ``update()`` works same as in :class:`StreamVGram`.
You can fit dictionary by ``fit()`` and continue fitting by ``accept``.
It's not recommended way but may be useful in specific cases.

``save(filename="", tokenizer=None)`` consume filename where dictionary will be saved and optionally tokenizer for more interpretable dictionary file.
If ``filename`` not passed used ``filename`` from the constructor.

``alphabet()`` return a 2-d list of integers, where every list of integers is built v-grams.

``freqs()`` return a 1-d list of integers with frequencies of v-gram occurrence in data.

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

If ``tokenizer`` has passed to ``save`` method field ``text`` contains the text of v-gram in ``alphabet`` items.
Without ``tokenizer`` the text field will not be in the file.

After v-grams construction, you can analyze the resulting dictionary.

| ``alphabet`` is a list of v-gram objects
| ``freq`` is a frequency of v-gram occurrence in data.
| ``vec`` is a vector of language alphabet symbols for v-gram presentation.
| ``coder`` is a sequence of symbols as they occur in the data.
| ``size`` is a size of a dictionary
| ``min_prob``, ``fitted`` and ``freqs_computed`` are inside information for a model.
| ``fitted`` and ``freqs_computed`` provided only for VGramBuilder class.
