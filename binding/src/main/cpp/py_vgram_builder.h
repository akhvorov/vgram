//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <pybind11/pybind11.h>
#include <cpp/int_vgram_builder.h>
#include <cpp/seq_coder.h>
#include <pybind11/pytypes.h>
#include "json.h"
#include "base_tokenizer.h"
#include "py_stream_vgram_builder.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyVGramBuilder : public PyStreamVGramBuilder {
public:
    PyVGramBuilder(int size, int iter_num);
    PyVGramBuilder(int size, int iter_num, int verbose);
    PyVGramBuilder(int size, int iter_num, const std::string& filename);
    PyVGramBuilder(int size, int iter_num, const std::string& filename, int verbose);
    explicit PyVGramBuilder(const std::string& filename);
    PyVGramBuilder(const std::string& filename, int verbose);
    PyVGramBuilder* fit(const std::vector<IntSeq>& seqs, py::args args);
    std::vector<std::string> transform(const std::vector<IntSeq>& seqs, py::args args) const;

private:
    int iter_num_;
    bool fitted_ = false;
    bool freqs_computed_ = false;

    void recompute_freqs(const std::vector<IntSeq>& seqs);
    json dict_to_json(BaseTokenizer* tokenizer) const override;
};

#endif //DICT_EXPANSION_VGRAM_H