
//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "py_vgram_builder.h"
#include "py_stream_vgram_builder.h"

namespace py = pybind11;

void init_vgram_builders(py::module &m) {
    m.def("loadStreamVGramBuilder", &PyStreamVGramBuilder::load);
    m.def("loadVGramBuilder", &PyVGramBuilder::load);

    py::class_<PyStreamVGramBuilder>(m, "StreamVGramBuilder")
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def("accept", &PyStreamVGramBuilder::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &PyStreamVGramBuilder::parse)
            .def("update", &PyStreamVGramBuilder::update_dict)
            .def("save", &PyStreamVGramBuilder::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &PyStreamVGramBuilder::freqs)
            .def("alphabet", &PyStreamVGramBuilder::alphabet);

    py::class_<PyVGramBuilder>(m, "VGramBuilder")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def("save", &PyVGramBuilder::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("fit", &PyVGramBuilder::fit, py::arg("seqs") = "", py::arg("filename") = "",
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("transform", &PyVGramBuilder::transform)
            .def("accept", &PyStreamVGramBuilder::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &PyStreamVGramBuilder::parse)
            .def("update", &PyStreamVGramBuilder::update_dict)
            .def("freqs", &PyVGramBuilder::freqs)
            .def("alphabet", &PyVGramBuilder::alphabet);
}