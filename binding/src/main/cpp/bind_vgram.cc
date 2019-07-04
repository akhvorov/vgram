//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <src/main/cpp/interfaces/vgram_builders/string_stream_vgram.h>
#include <src/main/cpp/interfaces/loader.h>
#include "py_int_vgram_builder.h"
#include "py_vgram_builder.h"

namespace py = pybind11;

PYBIND11_MODULE(vgram, m) {
    m.doc() = "python lib for vgram features building";

    py::class_<vgram_core::IntStreamVGram>(m, "IntStreamVGram")
            .def(py::init<int, int>(), py::arg("size") = 10000, py::arg("verbose") = 0)
            .def_static("load", &vgram_core::Loader::load_int_stream_vgram)
            .def("accept", &vgram_core::IntStreamVGram::accept,
                 py::call_guard<py::scoped_ostream_redirect,
                         py::scoped_estream_redirect>())
            .def("parse", &vgram_core::IntStreamVGram::parse)
            .def("update", &vgram_core::IntStreamVGram::update_dict)
            .def("save", &vgram_core::IntStreamVGram::save, py::arg("filename") = "vgram_dict.json",
                 py::arg("tokenizer") = nullptr)
            .def("freqs", &vgram_core::IntStreamVGram::freqs)
            .def("alphabet", &vgram_core::IntStreamVGram::alphabet);

    py::class_<vgram_core::StringStreamVGram>(m, "StreamVGram")
            .def(py::init<int, int, bool, bool>(), py::arg("size") = 10000, py::arg("verbose") = 0,
                 py::arg("inner_words_split") = false, py::arg("words_are_tokens") = false)
            .def_static("load", &vgram_core::Loader::load_string_stream_vgram)
            .def("accept", &vgram_core::StringStreamVGram::accept,
                 py::call_guard<py::scoped_ostream_redirect, py::scoped_estream_redirect>())
            .def("parse", &vgram_core::StringStreamVGram::parse)
            .def("update", &vgram_core::StringStreamVGram::update_dict)
            .def("save", &vgram_core::StringStreamVGram::save, py::arg("filename") = "vgram_dict.json")
            .def("freqs", &vgram_core::StringStreamVGram::freqs)
            .def("alphabet", &vgram_core::StringStreamVGram::alphabet);

    py::class_<PyIntVGram>(m, "IntVGram")
            .def(py::init<int, int, int>(), py::arg("size") = 10000, py::arg("iter_num") = 20, py::arg("verbose") = 0)
            .def_static("load", &PyIntVGram::load)
            .def("fit", &PyIntVGram::fit,
                 py::return_value_policy::reference,
                 py::call_guard<py::scoped_ostream_redirect, py::scoped_estream_redirect>())
            .def("transform", &PyIntVGram::transform_to_string)
            .def("save", &PyIntVGram::save, py::arg("filename") = "vgram_dict.json", py::arg("tokenizer") = nullptr)
            .def("freqs", &PyIntVGram::freqs)
            .def("alphabet", &PyIntVGram::alphabet);

    py::class_<PyStringVGram>(m, "VGram")
            .def(py::init<int, int, int>(), py::arg("size") = 10000, py::arg("iter_num") = 20, py::arg("verbose") = 0)
            .def_static("load", &PyStringVGram::load)
            .def("fit", &PyStringVGram::fit,
                 py::return_value_policy::reference,
                 py::call_guard<py::scoped_ostream_redirect, py::scoped_estream_redirect>())
            .def("transform", &PyStringVGram::transform)
            .def("save", &PyStringVGram::save, py::arg("filename") = "vgram_dict.json")
            .def("freqs", &PyStringVGram::freqs)
            .def("alphabet", &PyStringVGram::alphabet);
}
