
//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "vgram_builders/py_vgram_builder.h"
#include "vgram_builders/py_stream_vgram_builder.h"

namespace py = pybind11;

void init_vgram_builders(py::module &m) {
    m.def("loadIntStreamVGram", &PyIntStreamVGramBuilder::load);
    m.def("loadStreamVGram", &PyStreamVGramBuilder::load);
    m.def("loadIntVGram", &PyIntVGramBuilder::load);
    m.def("loadVGram", &PyVGramBuilder::load);

    py::class_<PyIntStreamVGramBuilder>(m, "IntStreamVGram")
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def("accept", &PyIntStreamVGramBuilder::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &PyIntStreamVGramBuilder::parse)
            .def("update", &PyIntStreamVGramBuilder::update_dict)
            .def("save", &PyIntStreamVGramBuilder::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &PyIntStreamVGramBuilder::freqs)
            .def("alphabet", &PyIntStreamVGramBuilder::alphabet);

    py::class_<PyStreamVGramBuilder>(m, "StreamVGram")
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def("accept", &PyStreamVGramBuilder::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &PyStreamVGramBuilder::parse)
            .def("update", &PyStreamVGramBuilder::update_dict)
            .def("save", &PyStreamVGramBuilder::save)
            .def("freqs", &PyStreamVGramBuilder::freqs)
            .def("alphabet", &PyStreamVGramBuilder::alphabet);

    py::class_<PyIntVGramBuilder>(m, "IntVGram")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def("save", &PyIntVGramBuilder::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &PyIntVGramBuilder::freqs)
            .def("fit", &PyIntVGramBuilder::fit,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())

            .def("transform", &PyIntVGramBuilder::transform_to_string)
            .def("freqs", &PyIntVGramBuilder::freqs)
            .def("alphabet", &PyIntVGramBuilder::alphabet);

    py::class_<PyVGramBuilder>(m, "VGram")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def("save", &PyVGramBuilder::save)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("fit", &PyVGramBuilder::fit,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("transform", &PyVGramBuilder::transform)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("alphabet", &PyVGramBuilder::alphabet);
}