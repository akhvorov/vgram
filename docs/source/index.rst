.. vgram documentation master file, created by
   sphinx-quickstart on Wed Oct 31 13:25:01 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

V-Gram features construction library
####################################


.. Indices and tables
.. ==================

.. * :ref:`genindex`
.. * :ref:`modindex`
.. * :ref:`search`

About
=====

This is implementation of new method for constructing an optimal feature set from sequential data.
It creates a dictionary of n-grams of variable length, based on the minimum description length principle.
The method is a dictionary coder and works simultaneously as both a compression algorithm and as unsupervised feature extraction.
The length of constructed v-grams is not limited by any bound and exceeds 100 characters in provided experiments.
Constructed v-grams can be used for any sequential data analysis and allows transfer bag-of-word techniques to non-text data types.
Extracted features generate a practical basis for text classification,
that shows competitive results on standard text classification collections without using the text structure.
Combining extracted character v-grams with the words from the original text we achieved substantially better
classification quality than on words or v-grams alone.

See the CIKM '18 `paper <http://delivery.acm.org/10.1145/3280000/3271789/p1343-kuralenok.pdf?ip=195.144.231.198&id=3271789&acc=OPENTOC&key=4D4702B0C3E38B35%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35%2E9F04A3A78F7D3B8D&__acm__=1540981038_f17e4a73acd10287dc73bbfe925f3432>`_ for details.

Install
=======

Install vgram by running:

.. code-block:: bash

	pip install vgram

Maybe you keep some errors about not installed pybind11 but it is okay.

Contents:
=========

.. toctree::
    :maxdepth: 2

    about
    builder
    tokenizers
    examples


Contribute
==========

- Source Code: github.com/akhvorov/vgram
- Issue Tracker: github.com/akhvorov/vgram/issues

License
=======

The project is licensed under the MIT license.
