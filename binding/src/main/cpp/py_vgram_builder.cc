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

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, int verbose) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1, verbose));
    size_ = size;
    dict_ = nullptr;
    coder_ = SeqCoder();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    iter_num_ = iter_num;
    fitted_ = false;
    verbose_ = verbose;
}

PyVGramBuilder::PyVGramBuilder(const std::string& filename) : PyVGramBuilder(filename, 1) {}

PyVGramBuilder::PyVGramBuilder(const std::string& filename, int verbose) {
    coder_ = SeqCoder();
    std::ifstream file(filename);
    std::vector<IntSeq> seqs;
    json dict;
    file >> dict;
    file.close();
    size_ = dict["size"];
    fitted_ = dict["fitted"].get<bool>();
    freqs_computed_ = dict["freqs_computed"].get<bool>();
    for (int n : dict["coder"]) {
        coder_.encode(std::vector<int>(1, n));
    }
    for (const auto& word_obj : dict["alphabet"]) {
        freqs_.push_back(word_obj["freq"].get<int>());
        seqs.push_back(word_obj["vec"].get<IntSeq>());
    }
    total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
    verbose_ = verbose;
}

json PyVGramBuilder::dict_to_json(BaseTokenizer* tokenizer) const {
    json dict;
    dict["size"] = size_;
    dict["fitted"] = fitted_;
    dict["freqs_computed"] = freqs_computed_;
    dict["coder"] = coder_to_json();
    dict["alphabet"] = alphabet_to_json(tokenizer);
    return dict;
}

json PyVGramBuilder::coder_to_json() const {
    std::vector<std::pair<int, int>> pairs;
    for (const auto& p : coder_.code_map()) {
        pairs.emplace_back(p);
    }
    std::sort(pairs.begin(), pairs.end(), [](const std::pair<int,int>& a, const std::pair<int,int>& b) { return a.second < b.second; });
    std::string res;
    json coder;
    for (const auto& p : pairs) {
        coder.push_back(p.first);
    }
    return coder;
}

json PyVGramBuilder::alphabet_to_json(BaseTokenizer* tokenizer) const {
    json alpha;
    for (std::size_t i = 0; i < dict_->alphabet().size(); i++) {
        auto word = dict_->alphabet()[i];
        json word_obj;
        word_obj["vec"] = json(word);
        word_obj["freq"] = freqs_[i];
        if (tokenizer != nullptr)
            word_obj["text"] = tokenizer->decode(std::vector<std::vector<int>>(1, coder_.decode(dict_->get(i))))[0];
        alpha.push_back(word_obj);
    }
    return alpha;
}

void PyVGramBuilder::save(const std::string& filename, BaseTokenizer* tokenizer) const {
    std::ofstream file(filename);
    file << std::setw(2) << dict_to_json(tokenizer) << std::endl;
    file.close();
}

IntSeq PyVGramBuilder::freqs() const {
    return freqs_;
}

PyVGramBuilder* PyVGramBuilder::fit(const std::vector<IntSeq>& seqs, py::args args) {
    if (fitted_) {
        if (freqs_computed_) {
            return this;
        }
        recompute_freqs(seqs);
        return this;
    }
    for (int i = 0; i < iter_num_; i++) {
        if (verbose_ > 0) {
            std::cout << i << "-th iteration" << std::endl;
        }
        for (int k = 0; k < seqs.size(); k++) {
            IntSeq s = coder_.encode(seqs[rand() % static_cast<int>(seqs.size())]);
            builder_->accept(s);
            if (PyErr_CheckSignals() == -1) {
                return nullptr;
            }
        }
        update_dict();
        save(filename_);
    }
    fitted_ = true;
    recompute_freqs(seqs);
    freqs_computed_ = true;
    return this;
}

void PyVGramBuilder::update_dict() {
    dict_ = builder_->result();
    builder_->result_freqs(&freqs_);
    total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
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
        IntSeq result;
        std::string str;
        dict_->parse(coder_.encode_immutable(seq), freqs_, total_freqs_, &result);
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

std::vector<IntSeq> PyVGramBuilder::alphabet() const {
    std::vector<IntSeq> alpha;
    for (const auto &a : dict_->alphabet()) {
        alpha.push_back(coder_.decode(a));
    }
    return alpha;
}
