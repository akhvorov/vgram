Save dictionary
###############

VGram builders allow save dictionary to file.
It's a good way to work with v-grams because dictionary is built for a long time.

Save dictionary by ``save`` method and load by static methods ``load[ClassName]``.

Format
======

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

Field ``text`` is not nessesary and provided only when you work with tests. It contains the text of v-gram in ``alphabet`` items.
In ``Int``-versions the text field will not be in the file.

After v-grams construction, you can analyze the resulting dictionary.

| ``alphabet`` is a list of v-gram objects
| ``freq`` is a frequency of v-gram occurrence in data.
| ``vec`` is a vector of language alphabet symbols for v-gram presentation.
| ``coder`` is a sequence of symbols as they occur in the data.
| ``size`` is a size of a dictionary
| ``min_prob``, ``fitted`` and ``freqs_computed`` are inner model information.
| ``fitted`` and ``freqs_computed`` provided only for ``(Int)VGram`` class.
