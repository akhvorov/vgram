.. _tokenizers:

Tokenizers
##########

VGramBuilder accepts a 2-d array of integers, but one of the most frequent usages is text analysis.
We should use tokenizers to encode data to integer arrays.

CharTokenizer
=============

For experiments, we use this tokenizer.
:class:`CharTokenizer` normalize text in the following way: all non-alphanumeric symbols were removed, then the text was converted to lower case.
After that text split on single chars.

In Python, it would look like this:

.. code-block:: python

    class CharTokenizer:
        def __init__(self): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def decode(self, X): ..

:class:`CharTokenizer` implements sklearn fit-transform interface.

``fit(X)`` consume a list of strings. Other arguments will be ignored.

``transform(X)`` consume a list of strings and return a 2-d list of integers. Other arguments will be ignored.

``fit_transform(X)`` consume a list of strings and return a 2-d list of integers. Other arguments will be ignored.
Combine fit and transform methods.

``decode(X)`` consume a 2-d list of integers and return list of strings.

This is a primary example of :class:`CharTokenizer` usage.

.. code-block:: python

    data = ["hello world", "other text", "blablabla"]
    tokenizer = CharTokenizer()
    transformed_data = tokenizer.fit(data).transform(data)
    print(transformed_data)

This tokenizer gives good results in experiments but for other tasks, different tokenizer may be more useful.

BaseTokenizer
=============

You can make your own tokenizer by inheritance from :class:`BaseTokenizer`.
You should only define normalize and tokenize methods for one string.

.. code-block:: python

    def BaseTokenizer():
        def __init__(self): ..
        def fit(self, X): ..
        def transform(self, X): ..
        def decode(self, X): ..
        def normalize(self, string): ..
        def tokenize(self, string): ..

``normalize(string)`` consume string and return a normalized string. If not redefined, the same string will be returned.

``tokenize(string)`` consume string and return a list of integers. If not redefined, list of characters will be returned.

Example of WordTokenizer implementation

.. code-block:: python

    class WordTokenizer(BaseTokenizer):
        def normalize(self, X):
            return [re.sub("[^ \w\d]", "", re.sub(" +", " ", x)).lower() for x in X]

        def tokenize(self, X):
            return [x.split(" ") for x in X]

:class:`BaseTokenizer` implements sklearn fit-transform interface.

``fit(X)`` consume a list of strings. Other arguments will be ignored.

``transform(X)`` consume a list of strings and return a 2-d list of integers. Other arguments will be ignored.

``fit_transform(X)`` consume a list of strings and return a 2-d list of integers. Other arguments will be ignored.
Combine fit and transform methods.

``decode(X)`` consume a 2-d list of integers and return list of strings.
