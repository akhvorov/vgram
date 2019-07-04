//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#ifndef DICT_EXPANSION_PY_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_VGRAM_BUILDER_H

#include <pybind11/pybind11.h>
#include <src/main/cpp/interfaces/vgram_builders/string_vgram.h>
#include <src/main/cpp/interfaces/loader.h>

namespace py = pybind11;

class PyStringVGram : public vgram_core::StringVGram {
public:
    static PyStringVGram *load(const std::string &filename) {
        return static_cast<PyStringVGram *>(vgram_core::Loader::load_string_vgram(filename));
    }

    PyStringVGram(int size, int iter_num) : vgram_core::StringVGram(size, iter_num) {}

    PyStringVGram(int size, int iter_num, int verbose) : vgram_core::StringVGram(size, iter_num, verbose) {}

    PyStringVGram(int size, int iter_num, vgram_core::BaseTokenizer *tokenizer)
            : vgram_core::StringVGram(size, iter_num, tokenizer) {}

    PyStringVGram(int size, int iter_num, vgram_core::BaseTokenizer *tokenizer, int verbose)
            : vgram_core::StringVGram(size, iter_num, tokenizer, verbose) {}

    PyStringVGram *fit(const std::vector<std::string> &seqs, /*const std::string &filename,*/ py::args &args) {
        return static_cast<PyStringVGram *>(vgram_core::StringVGram::fit(seqs));
    }

    std::vector<std::string> transform(const std::vector<std::string> &seqs, py::args &args) const {
        return vgram_core::StringVGram::transform(seqs);
    }
};

#endif //DICT_EXPANSION_PY_VGRAM_BUILDER_H
