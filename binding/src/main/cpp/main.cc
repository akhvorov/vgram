//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "py_vgram_builder.h"
#include "tokenizer.h"

namespace py = pybind11;

int add(int a, int b) {
    return 3;
}

//class PyTokenizer : public Tokenizer {
//public:
//    using Tokenizer::Tokenizer;
//    void fit(const std::vector<std::string>& seqs) override { PYBIND11_OVERLOAD(void, Tokenizer, fit, seqs); }
//    std::vector<std::vector<int>> transform(const std::vector<std::string>& seqs) override { PYBIND11_OVERLOAD(std::vector<std::vector<int>>, Tokenizer, transform, seqs); }
//};

PYBIND11_MODULE(vgram, m) {
    m.doc() = "python lib for vgram features building";
    m.def("add", &add, "A function which adds two numbers");
    py::class_<PyVGramBuilder>(m, "VGramBuilder")
            //.def(py::init<int, const IntSeq&, int>())
            // fix constructor!
//            .def("set", &Pet::set)
//            .def("set", (void (Pet::*)(int)) &Pet::set, "Set the pet's age")
//            .def("set", (void (Pet::*)(const std::string &)) &Pet::set, "Set the pet's name");
            .def(py::init<int, int>())
            .def(py::init<std::string>())
            .def("save", &PyVGramBuilder::save)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("fit", &PyVGramBuilder::fit)
            .def("transform", &PyVGramBuilder::transform)
            .def("alphabet", &PyVGramBuilder::alphabet);
    py::class_<Tokenizer>(m, "Tokenizer")
            .def(py::init<>())
            .def("fit", &Tokenizer::fit)
            .def("transform", &Tokenizer::transform);

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