
.. _vgram_builder:

V-Gram construction
###################

VGram
=====

For working with texts use ``VGram``.
This class implement the scikit-learn fit-transform interface and can be well embedded in the existing code.

.. code-block:: python

    class VGram(StreamVGram):
        def __init__(self, size=10000, iter_num=10, verbose=0): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def save(self, filename="vgram_dict.json"): ..
        def alphabet(self): ..
        def freqs(self): ..

| ``VGram(size=10000, iter_num=10, verbose=0)`` construct object, which can learn dictionary of size ``size`` in ``iter_num`` iterations.
| By default CharTokenizer is used (see :ref:`tokenizers`).
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``fit(X)`` consume a list of strings and return VGram object.
It makes ``iter_num`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume a list of strings.
Return a 2-d list of strings, where each row is partition of original string on v-grams.
It's good for pipeline where CountVectorizer follows VGram (see :ref:`examples`).

``save(filename="", tokenizer=None)`` consume filename where dictionary will be saved (see :ref:`save`).

``alphabet()`` return a list of v-grams.

``freqs()`` return a list of integers with frequencies of v-gram occurrence in data.

Load VGram from file by ``loadVGram``. ``loadVGram`` get file name and return VGram object.

If you work with streams, use StreamVGram (see :ref:`stream_builder`).
If you work with integer sequences, use `IntVGram_`

See :ref:`examples` for details.

IntVGram
========

For working with integers sequences use ``VGram``. It's similar to `VGram`_ but works with integers.
This class implement the scikit-learn fit-transform interface and can be well embedded in the existing code.

.. code-block:: python

    class IntVGram(StreamVGram):
        def __init__(self, size=10000, iter_num=10, verbose=0): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def save(self, filename="vgram_dict.json"): ..
        def alphabet(self): ..
        def freqs(self): ..

| ``IntVGram(size=10000, iter_num=10, verbose=0)`` construct object, which can learn dictionary of size ``size`` in ``iter_num`` iterations.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``fit(X)`` consume a 2-list of integers and return ``IntVGram`` object.
It makes ``iter_num`` iterations on all data to fit dictionary better. One iteration often is not enough.

``transform(X)`` consume a 2-d list of integers or 2-d numpy array.
Return a list of strings, where each row is consist of integers separate by stace. Each integer is id of v-gram in dictionary.
It's good for pipeline where CountVectorizer follows IntVGram even if you work with integers (see :ref:`examples`).
You can get integer list by apply ``split()`` to every element of returned list.

``save(filename="vgram_dict.json")`` consume filename where dictionary will be saved (see :ref:`save`).

``alphabet()`` return a 2-d list of integers where every row is a list of index of the corresponding v-gram.

``freqs()`` return a list of integers with frequencies of v-gram occurrence in data.

Load IntVGram from file by ``loadIntVGram``. ``loadIntVGram`` get file name and return IntVGram object.

If you work with streams, use IntStreamVGram (see :ref:`stream_builder`).

See :ref:`examples` for details.
