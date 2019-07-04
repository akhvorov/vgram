//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <pybind11/pybind11.h>
#include <src/main/cpp/interfaces/vgram_builders/int_vgram.h>
#include <src/main/cpp/interfaces/loader.h>

namespace py = pybind11;

class PyIntVGram : public vgram_core::IntVGram {
public:
    static PyIntVGram *load(const std::string &filename) {
        return static_cast<PyIntVGram*>(vgram_core::Loader::load_int_vgram(filename));
    }

    PyIntVGram(int size, int iter_num) : vgram_core::IntVGram(size, iter_num) {}

    PyIntVGram(int size, int iter_num, int verbose) : vgram_core::IntVGram(size, iter_num, verbose) {}

    PyIntVGram *fit(const std::vector<std::vector<int>> &seqs, /*const std::string &filename,*/ py::args &args) {
        return static_cast<PyIntVGram *>(vgram_core::IntVGram::fit(seqs));
    }

    std::vector<std::string> transform_to_string(const std::vector<std::vector<int>> &seqs, py::args &args) const {
        return vgram_core::IntVGram::transform_to_string(seqs);
    }

    bool isTerminated() const override {
        //TODO: check that it used
        return PyErr_CheckSignals() == -1;
    }
};

#endif //DICT_EXPANSION_VGRAM_H
