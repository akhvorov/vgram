//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#ifndef DICT_EXPANSION_TOKENIZER_H
#define DICT_EXPANSION_TOKENIZER_H

#include <string>
#import <vector>
#include <unordered_map>
#include <pybind11/pytypes.h>

namespace py = pybind11;

class BaseTokenizer {
public:
    BaseTokenizer() = default;

    explicit BaseTokenizer(const std::unordered_map<std::string, int> &forward_map);

    virtual ~BaseTokenizer() = default;

    BaseTokenizer *fit(const std::vector<std::string> &seqs, py::args args);

    std::vector<std::vector<int>> transform(const std::vector<std::string> &seqs, py::args &args);

    std::vector<int> transform(const std::string &seq);

    std::vector<std::vector<int>> fit_transform(const std::vector<std::string> &seqs, py::args &args);

    std::vector<std::string> decode(const std::vector<std::vector<int>> &seqs) const;

    std::string decode(const std::vector<int> &seq) const;

    virtual std::string normalize(const std::string &s) const;

    virtual std::vector<std::string> tokenize(const std::string &s) const;

private:
    std::unordered_map<std::string, int> forward_coder_;
    std::unordered_map<int, std::string> backward_coder_;
};

#endif //DICT_EXPANSION_TOKENIZER_H
