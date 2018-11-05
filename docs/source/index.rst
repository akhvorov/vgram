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

Vgram is the implementation of the new method for constructing an optimal feature set from sequential data.
It creates a dictionary of n-grams of variable length, based on the minimum description length principle.
The method is a dictionary coder and works simultaneously as both a compression algorithm and as unsupervised feature extraction.
The length of constructed v-grams is not limited by any bound and exceeds 100 characters in provided experiments.
Constructed v-grams can be used for any sequential data analysis and allows transfer bag-of-word techniques to non-text data types.
Extracted features generate a practical basis for text classification,
that shows competitive results on standard text classification collections without using the text structure.
Combining extracted character v-grams with the words from the original text we achieved substantially better
classification quality than on words or v-grams alone.

See the CIKM '18 `paper <https://dl.acm.org/citation.cfm?id=3271789>`_ for details.

Igor Kuralenok, Natalia Starikova, Aleksandr Khvorov, and Julian Serdyuk.
Construction of Efficient V-Gram Dictionary for Sequential Data Analysis,
CIKM '18 Proceedings of the 27th ACM International Conference on Information and Knowledge Management,
Pages 1343-1352

Install
=======

Install vgram by running:

.. code-block:: bash

	pip install vgram

Maybe you keep some errors about not installed pybind11, but it is okay.

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

- Source `Code <https://github.com/akhvorov/vgram>`_
- Issue `Tracker <https://github.com/akhvorov/vgram/issues>`_

License
=======

The project is licensed under the MIT license.
