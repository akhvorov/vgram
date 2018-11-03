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
#include <cpp/int_vgram_builder_p.h>
#include <cpp/int_dict_p.h>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) : PyVGramBuilder(size, iter_num, 1) {}

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, int verbose) : PyVGramBuilder(size, iter_num, "", verbose) {}

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, const std::string& filename)
: PyVGramBuilder(size, iter_num, filename, 1) {}

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, const std::string& filename, int verbose)
: PyStreamVGramBuilder(size, filename, 1) {
    iter_num_ = iter_num;
    fitted_ = false;
}

PyVGramBuilder::PyVGramBuilder(const std::string& filename) : PyVGramBuilder(filename, 1) {}

PyVGramBuilder::PyVGramBuilder(const std::string& filename, int verbose) : PyStreamVGramBuilder(filename, verbose) {
    json dict = read_dict(filename);
    fitted_ = dict["fitted"].get<bool>();
    freqs_computed_ = dict["freqs_computed"].get<bool>();
}

json PyVGramBuilder::dict_to_json(BaseTokenizer* tokenizer) const {
    json dict = PyStreamVGramBuilder::dict_to_json(tokenizer);
    dict["fitted"] = fitted_;
    dict["freqs_computed"] = freqs_computed_;
    return dict;
}

PyVGramBuilder* PyVGramBuilder::fit(const std::vector<IntSeq>& seqs, py::args args) {
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
        if (!filename_.empty()) {
            save(filename_);
        }
    }
    fitted_ = true;
    recompute_freqs(seqs);
    freqs_computed_ = true;
    return this;
}

void PyVGramBuilder::recompute_freqs(const std::vector<IntSeq>& seqs) {
    if (verbose_ > 0) {
        std::cout << "recompute freqs" << std::endl;
    }
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    for (const IntSeq& seq : seqs) {
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

std::vector<std::string> PyVGramBuilder::transform(const std::vector<IntSeq>& seqs, py::args args) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const IntSeq& seq : seqs) {
        IntSeq result = parse(seq);
        std::string str;
        for (const auto& i : result) {
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
