//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#ifndef DICT_EXPANSION_PY_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_VGRAM_BUILDER_H

#include <pybind11/pybind11.h>
#include <src/main/cpp/interfaces/vgram_builders/string_vgram.h>

namespace py = pybind11;

class StringVGram : public vgram_core::StringVGram {
public:
    static StringVGram *load(const std::string &filename) {
        return static_cast<StringVGram *>(vgram_core::StringVGram::load(filename));
    }

    StringVGram(int size, int iter_num) : vgram_core::StringVGram(size, iter_num) {}

    StringVGram(int size, int iter_num, int verbose) : vgram_core::StringVGram(size, iter_num, verbose) {}

    StringVGram(int size, int iter_num, vgram_core::BaseTokenizer *tokenizer)
            : vgram_core::StringVGram(size, iter_num, tokenizer) {}

    StringVGram(int size, int iter_num, vgram_core::BaseTokenizer *tokenizer, int verbose)
            : vgram_core::StringVGram(size, iter_num, tokenizer, verbose) {}

    StringVGram *fit(const std::vector<std::string> &seqs, /*const std::string &filename,*/ py::args &args) {
        return dynamic_cast<StringVGram *>(vgram_core::StringVGram::fit(seqs));
    }

    std::vector<std::string> transform(const std::vector<std::string> &seqs, py::args &args) const {
        return vgram_core::StringVGram::transform(seqs);
    }
};

#endif //DICT_EXPANSION_PY_VGRAM_BUILDER_H
