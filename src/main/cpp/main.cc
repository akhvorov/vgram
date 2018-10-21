//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "py_vgram_builder.h"
#include "base_tokenizer.h"
#include "char_tokenizer.h"

namespace py = pybind11;

int add(int a, int b) {
    return 3;
}

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

PYBIND11_MODULE(vgram, m) {
    m.doc() = "python lib for vgram features building";
    m.def("add", &add, "A function which adds two numbers");
    py::class_<PyVGramBuilder>(m, "VGramBuilder")
            .def(py::init<int, int>())
            .def(py::init<std::string>())
            .def("save", &PyVGramBuilder::save)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("fit", &PyVGramBuilder::fit)
            .def("transform", &PyVGramBuilder::transform)
            .def("alphabet", &PyVGramBuilder::alphabet);

    py::class_<BaseTokenizer, PyBaseTokenizer> base_tokenizer(m, "BaseTokenizer");
    base_tokenizer
            .def(py::init<>())
            .def("fit", &BaseTokenizer::fit)
            .def("transform", &BaseTokenizer::transform)
            .def("normalize", &BaseTokenizer::normalize)
            .def("tokenize", &BaseTokenizer::tokenize)
//            .def("decode", (std::vector<std::string> (BaseTokenizer::*)(const std::vector<std::vector<int>>&, py::args)) &BaseTokenizer::decode)
//            .def("decode", (std::vector<std::string> (BaseTokenizer::*)(const std::vector<std::string>&, py::args)) &BaseTokenizer::decode)
            .def("decode", &BaseTokenizer::decode)
            ;

    py::class_<CharTokenizer, PyCharTokenizer>(m, "CharTokenizer", base_tokenizer)
            .def(py::init<>())
//            .def("fit", &CharTokenizer::fit)
//            .def("transform", &CharTokenizer::transform)
            .def("normalize", &CharTokenizer::normalize)
            .def("tokenize", &CharTokenizer::tokenize)
//            .def("decode", &BaseTokenizer::decode)
            ;

//    py::class_<Tokenizer>(m, "Tokenizer")
//            .def(py::init<>())
//            .def("fit", &Tokenizer::fit)
//            .def("transform", &Tokenizer::transform);

//    py::class_<Tokenizer>(m, "Tokenizer");
//    py::class_<SimpleTokenizer, Tokenizer>(m, "SimpleTokenizer")
//            .def(py::init<>())
//            .def("fit", &SimpleTokenizer::fit)
//            .def("transform", &SimpleTokenizer::transform);

//    py::class_<Tokenizer, PyTokenizer>(m, "Tokenizer")
//            .def(py::init<>())
//            .def("fit", &Tokenizer::fit)
//            .def("transform", &Tokenizer::transform);
//
//    py::class_<SimpleTokenizer, Tokenizer>(m, "SimpleTokenizer")
//            .def(py::init<char>());
}
