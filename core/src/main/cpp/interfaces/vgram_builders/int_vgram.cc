
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
#include <int_vgram_builder_p.h>
#include <int_dict_p.h>
#include "int_vgram.h"

using namespace vgram_core;

IntVGram::IntVGram(int size, int iter_num) : IntVGram(size, iter_num, 1) {}

IntVGram::IntVGram(int size, int iter_num, int verbose)
        : IntStreamVGram(size, verbose) {
    iter_num_ = iter_num;
    fitted_ = false;
}

IntVGram *IntVGram::fit(const std::vector<IntSeq> &seqs/*, const std::string &filename,*/) {
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
            if (isTerminated()) {
                return nullptr;
            }
        }
        update_dict();
        //TODO: return it!
//        if (!filename.empty()) {
//            save(filename);
//        }
    }
    fitted_ = true;
    recompute_freqs(seqs);
    freqs_computed_ = true;
    return this;
}

void IntVGram::recompute_freqs(const std::vector<IntSeq> &seqs) {
    if (verbose_ > 0) {
        std::cout << "recompute freqs" << std::endl;
    }
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    for (const IntSeq &seq : seqs) {
        IntSeq result;
        dict_->parse(coder_->encode(seq), freqs, total_freqs, &result);
        for (int symb : result) {
            if (symb >= 0) {
                freqs_[symb]++;
            }
        }
        if (isTerminated()) {
            return;
        }
    }
    total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
}

std::vector<std::string> IntVGram::transform_to_string(const std::vector<IntSeq> &seqs) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const IntSeq &seq : transform(seqs)) {
        std::string str;
        for (const auto &i : seq) {
            str += " " + std::to_string(i);
        }
        if (str.length() == 0) {
            str = "-1";
        }
        res.push_back(str);
        if (isTerminated()) {
            break;
        }
    }
    return res;
}

std::vector<IntSeq> IntVGram::transform(const std::vector<IntSeq> &seqs) const {
    std::vector<IntSeq> res;
    res.reserve(seqs.size());
    for (const IntSeq &seq : seqs) {
        IntSeq result = parse(seq);
        res.push_back(result);
        if (isTerminated()) {
            break;
        }
    }
    return res;
}

json IntVGram::dict_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const {
    json dict = IntStreamVGram::dict_to_json(tokenizer);
    dict["fitted"] = fitted_;
    dict["freqs_computed"] = freqs_computed_;
    return dict;
}

bool IntVGram::isTerminated() const {
    return false;
}

IntVGram::IntVGram(std::shared_ptr<SeqCoder> coder, const IntSeq &freqs,
                   const std::vector<IntSeq> &alphabet, int size, double min_probability,
                   bool fitted, bool freqs_computed)
        : IntStreamVGram(size) {
    size_ = size;
    min_probability_ = min_probability;
    freqs_ = freqs;
    total_freqs_ = std::accumulate(freqs.begin(), freqs.end(), 0);
    coder_ = coder; //TODO: check it
    dict_ = std::make_shared<IntDictImpl>(alphabet);
    builder_ = std::make_shared<IntVGramBuilderImpl>(*dict_, freqs, alphabet, min_probability, 1);
    fitted_ = fitted;
    freqs_computed_ = freqs_computed;
    iter_num_ = 0;
}
