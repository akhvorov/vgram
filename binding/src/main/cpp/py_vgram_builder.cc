#include <utility>

//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <src/main/cpp/int_vgram_builder_p.h>
#include <src/main/cpp/int_dict_p.h>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) : PyVGramBuilder(size, iter_num, 1) {}

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, int verbose)
        : PyStreamVGramBuilder(size, verbose) {
    iter_num_ = iter_num;
    fitted_ = false;
}

PyVGramBuilder *PyVGramBuilder::fit(const std::vector<IntSeq> &seqs, const std::string &filename, py::args args) {
    if (fitted_) {
        if (freqs_computed_) {
            return this;
        }
        recompute_freqs(seqs);
        freqs_computed_ = true;
        return this;
    }
    for (int i = 0; i < iter_num_; i++) {
        if (verbose_ > 0) {
            std::cout << i << "-th iteration" << std::endl;
        }
        for (int k = 0; k < seqs.size(); k++) {
            accept(seqs[rand() % static_cast<int>(seqs.size())]);
            if (PyErr_CheckSignals() == -1) {
                return nullptr;
            }
        }
        update_dict();
        if (!filename.empty()) {
            save(filename);
        }
    }
    fitted_ = true;
    recompute_freqs(seqs);
    freqs_computed_ = true;
    return this;
}

void PyVGramBuilder::recompute_freqs(const std::vector<IntSeq> &seqs) {
    if (verbose_ > 0) {
        std::cout << "recompute freqs" << std::endl;
    }
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    for (const IntSeq &seq : seqs) {
        IntSeq result;
        dict_->parse(coder_.encode(seq), freqs, total_freqs, &result);
        for (int symb : result) {
            if (symb >= 0) {
                freqs_[symb]++;
            }
        }
        if (PyErr_CheckSignals() == -1) {
            return;
        }
    }
    total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
}

std::vector<std::string> PyVGramBuilder::transform(const std::vector<IntSeq> &seqs, py::args args) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const IntSeq &seq : seqs) {
        IntSeq result = parse(seq);
        std::string str;
        for (const auto &i : result) {
            str += " " + std::to_string(i);
        }
        if (str.length() == 0) {
            str = "-1";
        }
        res.push_back(str);
        if (PyErr_CheckSignals() == -1) {
            break;
        }
    }
    return res;
}

json PyVGramBuilder::dict_to_json(BaseTokenizer *tokenizer) const {
    json dict = PyStreamVGramBuilder::dict_to_json(tokenizer);
    dict["fitted"] = fitted_;
    dict["freqs_computed"] = freqs_computed_;
    return dict;
}

PyVGramBuilder::PyVGramBuilder(const SeqCoder &coder, const IntSeq &freqs, const std::vector<IntSeq> &seqs,
                               const std::vector<IntSeq> &alphabet, int size, double min_probability, bool fitted,
                               bool freqs_computed) : PyStreamVGramBuilder(size) {
    size_ = size;
    min_probability_ = min_probability;
    freqs_ = freqs;
    total_freqs_ = std::accumulate(freqs.begin(), freqs.end(), 0);
    coder_ = coder;
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(*dict_, freqs, alphabet, min_probability, 1));
    fitted_ = fitted;
    freqs_computed_ = freqs_computed;
}
