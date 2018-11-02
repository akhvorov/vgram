//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "py_vgram_builder.h"
#include "base_tokenizer.h"
#include "char_tokenizer.h"

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

PYBIND11_MODULE(vgram, m) {
    m.doc() = "python lib for vgram features building";
    py::class_<PyVGramBuilder>(m, "VGramBuilder")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def(py::init<std::string>())
            .def(py::init<std::string, int>())
            .def("save", &PyVGramBuilder::save)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("fit", &PyVGramBuilder::fit,
                    py::call_guard<py::scoped_ostream_redirect,
                    py::scoped_estream_redirect>())
            .def("transform", &PyVGramBuilder::transform)
            .def("alphabet", &PyVGramBuilder::alphabet);

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
