
.. _stream_builder:

Stream V-Gram construction
##########################

StreamVGram
===========

For working with streaming texts use ``StreamVGram``.

.. code-block:: python

 class StreamVGram:
     def __init__(self, size=10000, verbose=0): ..
     def accept(self, X): ..
     def parse(self, X): ..
     def update(self): ..
     def save(self, filename="vgram_dict.json"): ..
     def alphabet(self): ..
     def freqs(self): ..

| ``StreamVGram(size=10000, verbose=0)`` construct object, which can learn dictionary of size ``size`` in stream mode.
| By default CharTokenizer is used (see :ref:`tokenizers`).
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``accept(x)`` consume a strings and not return value.

``parse(x)`` consume a strings and return v-gram representation of it.

``update()`` after accepting and before parsing make update of dictionary.
Otherwise, you will use unfitted dictionary.

``save(filename="vgram_dict.json")`` consume filename where dictionary will be saved (see :ref:`save`).

``alphabet()`` return a list of v-grams.

``freqs()`` return a list of integers with frequencies of v-gram occurrence in data.

Load StreamVGram from file by ``loadStreamVGram``. ``loadStreamVGram`` get file name and return StreamVGram object.

If your text collection is not so big and comes ones, more convinient use VGram (see :ref:`vgram_builder`).
If you work with integer sequences, use `IntStreamVGram_`

See :ref:`examples` for details.

IntStreamVGram
==============

For working with integers streaming sequences use ``IntStreamVGram``. It's similar to `StreamVGram`_ but works with integers.

.. code-block:: python

 class IntStreamVGram(StreamVGram):
     def __init__(self, size=10000, verbose=0): ..
     def accept(self, X): ..
     def parse(self, X): ..
     def update(self): ..
     def save(self, filename="vgram_dict.json"): ..
     def alphabet(self): ..
     def freqs(self): ..

| ``IntStreamVGram(size=10000, verbose=0)`` construct object, which can learn dictionary of size ``size``.
| ``verbose`` means a level of verbose. ``0`` not print anything, ``1`` or more print some useful information about v-gram learning process.

``accept(x)`` consume a list of integers and not return value.

``parse(x)`` consume a list of integers and return integer list of v-gram's id.
It's list of indices for alphabet.

``update()`` after accepting and before parsing make update of dictionary.
Otherwise, you will use unfitted dictionary.

``save(filename="vgram_dict.json")`` consume filename where dictionary will be saved (see :ref:`save`).

``alphabet()`` return a 2-d list of integers where every row is v-gram.

``freqs()`` return a list of integers with frequencies of v-gram occurrence in data.

Load IntStreamVGram from file by ``loadIntStreamVGram``. ``loadIntStreamVGram`` get file name and return IntStreamVGram object.

If you not work with streams, use IntVGram (see :ref:`vgram_builder`).

See :ref:`examples` for details.
