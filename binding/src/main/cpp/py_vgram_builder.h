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

namespace py = pybind11;
using json = nlohmann::json;

class PyVGramBuilder {
public:
    PyVGramBuilder(int size, int iter_num);
    PyVGramBuilder(int size, int iter_num, int verbose);
    explicit PyVGramBuilder(const std::string& filename);
    PyVGramBuilder(const std::string& filename, int verbose);
    void save(const std::string& filename, BaseTokenizer* tokenizer = nullptr) const;
    IntSeq freqs() const;
    PyVGramBuilder* fit(const std::vector<IntSeq>& seqs, py::args args);
    std::vector<std::string> transform(const std::vector<IntSeq>& seqs, py::args args) const;
    std::vector<IntSeq> alphabet() const;

private:
    int iter_num_;
    std::shared_ptr<IntVGramBuilder> builder_;
    std::shared_ptr<IntDict> dict_;
    SeqCoder coder_;
    IntSeq freqs_;
    int total_freqs_;
    bool fitted_ = false;
    int verbose_;

    void compute_freqs(const std::vector<IntSeq>& seqs);
};

#endif //DICT_EXPANSION_VGRAM_H