//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "py_vgram_builder.h"

namespace py = pybind11;

int add(int a, int b) {
    return 3;
}

PYBIND11_MODULE(vgram, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring
    m.def("add", &add, "A function which adds two numbers");
//    m.def("parse", &parse, "A function which adds two numbers");
//
//    py::class_<IntDict>(m, "IntDict");
//    py::class_<IntDictImpl, IntDict>(m, "IntDictImpl")
//            .def(py::init<>())
//            .def("size", &IntDictImpl::size)
//            //.def("parse", (int (IntDictImpl::*)(const IntSeq&, const IntSeq&, double, IntSeq*)) &IntDictImpl::parse, "parse seq");
//            .def("parse", (int (IntDictImpl::*)(const IntSeq&, const IntSeq&, double, IntSeq*)) &IntDictImpl::parse, "parse seq");
    py::class_<PyVGramBuilder>(m, "VGramBuilder")
            .def(py::init<int>())
            .def(py::init<const IntSeq&, int>())
            .def("accept", &PyVGramBuilder::accept)
            .def("parse", &PyVGramBuilder::parse)
            .def("alphabet", &PyVGramBuilder::alphabet);
}