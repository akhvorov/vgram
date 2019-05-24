//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#ifndef PY_VGRAM_BASE_TOKENIZER_H
#define PY_VGRAM_BASE_TOKENIZER_H

#include <pybind11/pytypes.h>
#include <src/main/cpp/interfaces/tokenizers/base_tokenizer.h>

namespace py = pybind11;

class BaseTokenizer : public vgram_core::BaseTokenizer {
public:
    BaseTokenizer *fit(const std::vector<std::string> &seqs, const py::args &args) {
        vgram_core::BaseTokenizer::fit(seqs);
        return this;
    }

    std::vector<std::vector<int>> transform(const std::vector<std::string> &seqs, py::args &args) {
        return vgram_core::BaseTokenizer::transform(seqs);
    }

    std::vector<std::vector<int>> fit_transform(const std::vector<std::string> &seqs, py::args &args) {
        return vgram_core::BaseTokenizer::fit_transform(seqs);
    }
};

#endif //PY_VGRAM_BASE_TOKENIZER_H
