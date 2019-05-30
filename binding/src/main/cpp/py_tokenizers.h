//
// Created by Aleksandr Khvorov on 29/05/2019.
//

#ifndef DICT_EXPANSION_PY_TOKENIZERS_H
#define DICT_EXPANSION_PY_TOKENIZERS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <src/main/cpp/interfaces/tokenizers/char_tokenizer.h>
#include "base_tokenizer.h"

namespace py = pybind11;

class PyBaseTokenizer : public BaseTokenizer {
public:
    using BaseTokenizer::BaseTokenizer;

    std::string normalize(const std::string &str) const override {
        PYBIND11_OVERLOAD(std::string, BaseTokenizer, normalize, str);
    }

    std::vector<std::string> tokenize(const std::string &str) const override {
        PYBIND11_OVERLOAD(std::vector<std::string>, BaseTokenizer, tokenize, str);
    }
};

class PyCharTokenizer : public BaseTokenizer, public vgram_core::CharTokenizer {
public:
    using CharTokenizer::CharTokenizer;

    std::string normalize(const std::string &str) const override {
        PYBIND11_OVERLOAD(std::string, vgram_core::CharTokenizer, normalize, str);
    }

    std::vector<std::string> tokenize(const std::string &str) const override {
        PYBIND11_OVERLOAD(std::vector<std::string>, vgram_core::CharTokenizer, tokenize, str);
    }
};

#endif //DICT_EXPANSION_PY_TOKENIZERS_H
