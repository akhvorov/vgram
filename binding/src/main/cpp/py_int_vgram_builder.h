//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <pybind11/pybind11.h>
#include <src/main/cpp/interfaces/vgram_builders/int_vgram.h>

namespace py = pybind11;

class IntVGram : public vgram_core::IntVGram {
public:
    static IntVGram *load(const std::string &filename) {
        return static_cast<IntVGram*>(vgram_core::IntVGram::load(filename));
    }

    IntVGram(int size, int iter_num) : vgram_core::IntVGram(size, iter_num) {}

    IntVGram(int size, int iter_num, int verbose) : vgram_core::IntVGram(size, iter_num, verbose) {}

    IntVGram *fit(const std::vector<std::vector<int>> &seqs, /*const std::string &filename,*/ py::args &args) {
        return dynamic_cast<IntVGram *>(vgram_core::IntVGram::fit(seqs));
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
