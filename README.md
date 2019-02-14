[![Documentation Status](https://readthedocs.org/projects/vgram/badge/?version=latest)](https://vgram.readthedocs.io/en/latest/?badge=latest)

# vgram

This is implementation of CIKM'18 paper [Construction of Efficient V-Gram Dictionary for Sequential Data Analysis](https://dl.acm.org/citation.cfm?id=3271789)
 
V-gram is the new method for constructing an optimal
feature set from sequential data. It creates a dictionary of n-grams
of variable length, based on the minimum
description length principle. The method is a dictionary
coder and works simultaneously as both a compression algorithm
and as unsupervised feature extraction. The length of constructed
v-grams is not limited by any bound and exceeds 100 characters in
provided experiments. Constructed v-grams can be used for any sequential
data analysis and allows transfer bag-of-word techniques
to non-text data types. Extracted features generate a
practical basis for text classification, that shows competitive results
on standard text classification collections without using the text
structure. Combining extracted character v-grams with the words
from the original text we achieved substantially better classification
quality than on words or v-grams alone.

### Install

```bash
pip install vgram
``` 
Also you should have Python 3 and cmake. Maybe you keep some errors about not installed pybind11 but it is okay.

### Examples

Basic example of 20 news groups dataset classification

```python
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
```

V-Gram is unsupervised method that's why we fit vgram to all data.
Once fitted, vgram don't fit again and we could not trouble about doubled fitting.  
In last two lines shown how to get dictionary alphabet and print some elements.
