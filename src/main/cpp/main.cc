//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "int_vgram_builder_p.h"
#include "int_dict_p.h"

namespace py = pybind11;

int add(int a, int b) {
    return a + b;
}

IntSeq parse(IntDict& dict, const IntSeq& seq, const IntSeq& freqs, double total_freq) {
    IntSeq result;
    int s = dict.parse(seq, freqs, total_freq, &result);
    return result;

}

PYBIND11_MODULE(vgram, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring
    m.def("add", &add, "A function which adds two numbers");
    m.def("parse", &parse, "A function which adds two numbers");

    py::class_<IntVGramBuilderImpl>(m, "IntVGramBuilderImpl")
            .def(py::init<int>())
            .def("accept", &IntVGramBuilderImpl::accept)
            .def("result", &IntVGramBuilderImpl::result);
}