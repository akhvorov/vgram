
.. _about:

Use cases and theory
####################

Data compression
================

V-Grams dictionary building based on minimum description length principle.
This allow us to achieve competitive results on data compression tasks.
Comparison of v-gram algorithm with other popular compression algorithms is presented below.

!Place picture here!

Text classification
===================

Dictionary of v-grams may be used as words in bag-of-words text representation.
From the practical perspective, v-grams has at least two significant advantages over words:
straightforward text normalization and fixed dictionary volume.
V-gram dictionary can be much smaller then words dictionary but demonstrate the same score.
At IMDb dataset classification dictionary of only 5000 vgrams reach better score then 75000 words (15 times smaller).

In our experiments we tested v-grams on three datasets: 20 News Groups, IMDb movie reviews, AG news.
Bag-of-words, bag-of-vgrams and bag-of-(words&vgrams) was compared.
Text was normalized in following way: all non alphanumeric symbols was removed, then the text was converted to lower case.
Experiments show that even small vgram dictionary extract additional information and significantly increase score.

!Place picture here!

Also bag-of-words methods can be applied to non textual data such as genome sequences or
text without words splitting like text in asian or arabic languages.

Sequences analysis
==================

V-Grams dictionary contains a lot of valuable information about domain.

TODO
