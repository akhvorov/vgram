import unittest

from vgram import BaseTokenizer


class TokenizerTest(unittest.TestCase):

    def test_init(self):
        tokenizer = BaseTokenizer()
        self.assertIsNotNone(tokenizer)

    def test_accept(self):
        tokenizer = BaseTokenizer()
        data = ["abc"]
        self.assertEqual(tokenizer.fit(data).transform(data), [[0, 1, 2]])

    def test_fit_twice(self):
        tokenizer = BaseTokenizer()
        data1 = ["abc"]
        data2 = ["cde"]
        tokenizer.fit(data1)
        self.assertEqual(tokenizer.transform(data1), [[0, 1, 2]])
        tokenizer.fit(data2)
        self.assertEqual(tokenizer.transform(data2), [[2, 3, 4]])


if __name__ == '__main__':
    unittest.main()
