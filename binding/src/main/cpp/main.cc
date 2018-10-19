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
        PYBIND11_OVERLOAD_PURE(
                std::string,
                BaseTokenizer,
                normalize,
                str
        );
    }

    std::vector<std::string> tokenize(const std::string& str) const override {
        PYBIND11_OVERLOAD_PURE(
                std::vector<std::string>, /* Return type */
                BaseTokenizer,      /* Parent class */
                tokenize,          /* Name of function in C++ (must match Python name) */
                str      /* Argument(s) */
        );
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

    py::class_<BaseTokenizer, PyBaseTokenizer>(m, "BaseTokenizer")
            .def(py::init<>())
            .def("fit", &BaseTokenizer::fit)
            .def("transform", &BaseTokenizer::transform)
            .def("normalize", &BaseTokenizer::normalize)
            .def("tokenize", &BaseTokenizer::tokenize);

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
