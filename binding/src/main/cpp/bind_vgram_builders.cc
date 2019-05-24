
//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <src/main/cpp/interfaces/vgram_builders/string_stream_vgram.h>
#include "base_tokenizer.h"
#include "py_int_vgram_builder.h"
#include "py_vgram_builder.h"

namespace py = pybind11;

void init_vgram_builders(py::module &m) {
    m.def("loadIntStreamVGram", &vgram_core::IntStreamVGram::load);
    m.def("loadStreamVGram", &vgram_core::StringStreamVGram::load);
    m.def("loadIntVGram", &IntVGram::load);
    m.def("loadVGram", &StringVGram::load);

    py::class_<vgram_core::IntStreamVGram>(m, "IntStreamVGram")
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def("accept", &vgram_core::IntStreamVGram::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &vgram_core::IntStreamVGram::parse)
            .def("update", &vgram_core::IntStreamVGram::update_dict)
            .def("save", &vgram_core::IntStreamVGram::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &vgram_core::IntStreamVGram::freqs)
            .def("alphabet", &vgram_core::IntStreamVGram::alphabet);

    py::class_<vgram_core::StringStreamVGram>(m, "StringStreamVGram")
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def(py::init<int, BaseTokenizer*>())
            .def(py::init<int, BaseTokenizer*, int>())
            .def("accept", &vgram_core::StringStreamVGram::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &vgram_core::StringStreamVGram::parse)
            .def("update", &vgram_core::StringStreamVGram::update_dict)
            .def("save", &vgram_core::StringStreamVGram::save)
            .def("freqs", &vgram_core::StringStreamVGram::freqs)
            .def("alphabet", &vgram_core::StringStreamVGram::alphabet);

    py::class_<IntVGram>(m, "IntVGram")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def("save", &IntVGram::save, py::arg("filename") = "", py::arg("tokenizer") = nullptr)
            .def("freqs", &IntVGram::freqs)
            .def("fit", &IntVGram::fit,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())

            .def("transform", &IntVGram::transform_to_string)
            .def("freqs", &IntVGram::freqs)
            .def("alphabet", &IntVGram::alphabet);

    py::class_<StringVGram>(m, "VGram")
            .def(py::init<int, int>())
            .def(py::init<int, int, int>())
            .def(py::init<int, int, BaseTokenizer*>())
            .def(py::init<int, int, BaseTokenizer*, int>())
            .def("save", &StringVGram::save)
            .def("freqs", &StringVGram::freqs)
            .def("fit", &StringVGram::fit,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("transform", &StringVGram::transform)
            .def("freqs", &StringVGram::freqs)
            .def("alphabet", &StringVGram::alphabet);
}