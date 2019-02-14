//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "tokenizers/base_tokenizer.h"
#include "tokenizers/char_tokenizer.h"

namespace py = pybind11;

class PyBaseTokenizer : public BaseTokenizer {
public:
    using BaseTokenizer::BaseTokenizer;

    std::string normalize(const std::string& str) const override {
        PYBIND11_OVERLOAD(std::string, BaseTokenizer, normalize, str);
    }

    std::vector<std::string> tokenize(const std::string& str) const override {
        PYBIND11_OVERLOAD(std::vector<std::string>, BaseTokenizer, tokenize, str);
    }
};

class PyCharTokenizer : public CharTokenizer {
public:
    using CharTokenizer::CharTokenizer;

    std::string normalize(const std::string& str) const override {
        PYBIND11_OVERLOAD(std::string, CharTokenizer, normalize, str);
    }

    std::vector<std::string> tokenize(const std::string& str) const override {
        PYBIND11_OVERLOAD(std::vector<std::string>, CharTokenizer, tokenize, str);
    }
};

void init_tokenizers(py::module &m) {
    py::class_<BaseTokenizer, PyBaseTokenizer> base_tokenizer(m, "BaseTokenizer");
    base_tokenizer
            .def(py::init<>())
            .def("fit", &BaseTokenizer::fit)
            .def("transform", &BaseTokenizer::transform)
            .def("fit_transform", &BaseTokenizer::fit_transform)
            .def("normalize", &BaseTokenizer::normalize)
            .def("tokenize", &BaseTokenizer::tokenize)
            .def("decode", &BaseTokenizer::decode)
            ;

    py::class_<CharTokenizer, PyCharTokenizer>(m, "CharTokenizer", base_tokenizer)
            .def(py::init<>())
            .def("normalize", &CharTokenizer::normalize)
            .def("tokenize", &CharTokenizer::tokenize)
            ;
}
