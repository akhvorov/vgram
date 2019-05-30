//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <src/main/cpp/interfaces/tokenizers/char_tokenizer.h>
#include "py_tokenizers.h"

namespace py = pybind11;

void init_tokenizers(py::module &m) {
    py::class_<BaseTokenizer, PyBaseTokenizer> base_tokenizer(m, "BaseTokenizer");
    base_tokenizer
            .def(py::init<>())
            .def("fit", &BaseTokenizer::fit)
            .def("transform",
                 py::overload_cast<const std::vector<std::string> &, py::args &>(&BaseTokenizer::transform))
            .def("transform", py::overload_cast<const std::string &>(&vgram_core::BaseTokenizer::transform))
            .def("fit_transform", &BaseTokenizer::fit_transform)
            .def("normalize", &BaseTokenizer::normalize)
            .def("tokenize", &BaseTokenizer::tokenize)
            .def("decode", py::overload_cast<const std::vector<std::vector<int>> &>(&BaseTokenizer::decode, py::const_))
            .def("decode", py::overload_cast<const std::vector<int> &>(&BaseTokenizer::decode, py::const_));

    py::class_<vgram_core::CharTokenizer, PyCharTokenizer>(m, "CharTokenizer", base_tokenizer)
            .def(py::init<>())
            .def("normalize", &vgram_core::CharTokenizer::normalize)
            .def("tokenize", &vgram_core::CharTokenizer::tokenize);
}
