
.. _builder:

VGramBuilder
############

The main class where v-gram dictionary constructed is VGramBuilder.
If it was a class in Python, it would look like this:

.. code-block:: python

    class VGramBuilder:
        def __init__(self, size=10000, iters=10): ..
        # yes, two different constructors
        def __init__(self, filename): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def save(self, filename): ..
        def save(self, filename, tokenizer): ..
        def alphabet(self): ..

:class:`VGramBuilder` have two constructors. One for building new dictionary and second for using existing dictionary.
For using second constructor you should already build dictionary before and saved it.

VGramBuilder implement convenient sklearn fit-transform interface.

``VGramBuilder(size, iters)`` building new dictionary of size ``size`` and learn ``iters`` iterations.

``VGramBuilder(filename)`` building dictionary from file. It works if you fit dictionary and save to file before.

``fit(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.

``transform(X)`` consume 2-d list of integer or 2-d numpy array. Other arguments will be ignored.
Return 1-d list of strings, where each string is integers joined by space, where every integer is vgram is in alphabet.
It's good for pipeline where CountVectorizer follows VGramBuilder (see :ref:`examples`).

``save(filename, tokenizer=None)`` consume filename where dict will be save and optionally tokenizer for more interpretable dictionary file.

``alphabet()`` return 2-d list of integers, where every list of integers is build vgram.

If you work with integers streams, VGramBuilder is enough, but for text conveniently to use tokenizers (see :ref:`tokenizers`).

See :ref:`examples` for details.
