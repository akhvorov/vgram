[![Documentation Status](https://readthedocs.org/projects/vgram/badge/?version=latest)](https://vgram.readthedocs.io/en/latest/?badge=latest)
[![PyPI version](https://badge.fury.io/py/vgram.svg)](https://badge.fury.io/py/vgram)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

# vgram

This is implementation of CIKM'18 paper [Construction of Efficient V-Gram Dictionary for Sequential Data Analysis](https://dl.acm.org/citation.cfm?id=3271789)

`vgram` is a Python package, which provide sklearn-like fit-transform interface for easy integration into your pipeline.

`vgram` is similar to BPE (Sennrich et al., 2016), but instead of frequencies takes into account the informativeness of subwords.
This allows you to compress the dictionary to several thousand elements with increasing accuracy.

## Install

```bash
pip install vgram
``` 
Also you should have Python 3 and cmake. Maybe you keep some errors about not installed pybind11 but it is okay.

## Examples

### Basic usage

Fit vgram dictionary

```python
from vgram import VGram

texts = ["hello world", "a cat sat on the mat"]
vgram = VGram(size=20, iter_num=300)
vgram.fit(texts)
result = vgram.transform(texts)

```

### Working with integer sequences

`vgram` can be applied not only for text, but also to integer sequences.
This generalization allow works with non-textual data or transorm text to tokens by yourself.
This example is equivalent to previous.

```python
from vgram import IntVGram, CharTokenizer

texts = ["hello world", "a cat sat on the mat"]
tokenizer = CharTokenizer()

# transform text to tokens ids and pass to IntVGram
tok_texts = tokenizer.fit_transform(texts)
vgram = IntVGram(size=10000, iter_num=10)
vgram.fit(tok_texts)
result = tokenizer.decode(vgram.transform(tok_texts))

``` 

### Custom tokenization

By default `VGram` make all texts lowercase and remove all non-alphanumeric symbols amd split on characters. 
This normalizations is not good for many tasks, that's why you can fit vgram dictionary with custom normalization and tokenization. 
You should only derive class BaseTokenizer and implement normalize and tokenize methods. 

Note: This feature is not stable, use previous variant for custom tokenizers.

```python
from vgram import VGram, BaseTokenizer

class MyTokenizer(BaseTokenizer):
    def normalize(self, s):
        return s
        
    def tokenize(self, s):
        return s.split(' ')
        
texts = ["hello world", "a cat sat on the mat"]
tokenizer = MyTokenizer()
tok_vgram = VGram(size=10000, iter_num=10, tokenizer=tokenizer)
tok_vgram.fit(texts)
tok_result = tok_vgram.transform(texts)

```
You can change tokenization and try to build dictionary of vgrams, where words are symbols.  

### Classification pipeline

Basic example of 20 news groups dataset classification

```python
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
vgram = Pipeline([
    ("vgb", VGram(size=10000, iter_num=10)),
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
```

V-Gram is unsupervised method that's why we fit vgram to all data.
Once fitted, vgram don't fit again and we could not trouble about doubled fitting.  
In last two lines shown how to get dictionary alphabet and print some elements.

Read full [documentation](https://vgram.readthedocs.io) for more information.
