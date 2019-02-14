//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <src/main/cpp/int_dict_p.h>
#include <src/main/cpp/int_vgram_builder_p.h>
#include "py_stream_vgram_builder.h"
#include "stream_vgram_serializer.h"


PyStreamVGramBuilder::PyStreamVGramBuilder(int size)
        : PyStreamVGramBuilder(size, 1) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(int size, int verbose = 1) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1, verbose));
    size_ = size;
    dict_ = nullptr;
    coder_ = SeqCoder();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    verbose_ = verbose;
}

void PyStreamVGramBuilder::accept(const IntSeq &seq) {
    builder_->accept(coder_.encode(seq));
}

IntSeq PyStreamVGramBuilder::parse(const IntSeq &seq) const {
    IntSeq result;
    dict_->parse(coder_.encode_immutable(seq), freqs_, total_freqs_, &result);
    return result;
}

void PyStreamVGramBuilder::update_dict() {
    if (dict_ != builder_->result()) {
        dict_ = builder_->result();
        builder_->result_freqs(&freqs_);
        total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
        min_probability_ = builder_->get_min_probability();
    }
}

std::vector<IntSeq> PyStreamVGramBuilder::alphabet() const {
    std::vector<IntSeq> alpha;
    for (const auto &a : dict_->alphabet()) {
        alpha.push_back(coder_.decode(a));
    }
    return alpha;
}

IntSeq PyStreamVGramBuilder::freqs() const {
    return freqs_;
}

void PyStreamVGramBuilder::save(const std::string &filename, BaseTokenizer *tokenizer) const {
    std::ofstream file;
    if (filename.empty()) {
        std::cout << "Error: no filename for save. Pass filename to constructor or save method" << std::endl;
    } else {
        file = std::ofstream(filename);
    }
    file << std::setw(2) << dict_to_json(tokenizer) << std::endl;
    file.close();
}

json PyStreamVGramBuilder::dict_to_json(BaseTokenizer *tokenizer) const {
    json dict;
    dict["size"] = size_;
    dict["min_prob"] = min_probability_;
    dict["coder"] = coder_to_json();
    dict["alphabet"] = alphabet_to_json(tokenizer);
    return dict;
}

json PyStreamVGramBuilder::coder_to_json() const {
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

json PyStreamVGramBuilder::alphabet_to_json(BaseTokenizer* tokenizer) const {
    json alpha;
    for (int i = 0; i < dict_->alphabet().size(); i++) {
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

PyStreamVGramBuilder::PyStreamVGramBuilder(const SeqCoder &coder, const IntSeq &freqs, const std::vector<IntSeq> &seqs,
                                           const std::vector<IntSeq> &alphabet, int size, double min_probability) {
    size_ = size;
    min_probability_ = min_probability;
    freqs_ = freqs;
    total_freqs_ = std::accumulate(freqs.begin(), freqs.end(), 0);
    coder_ = coder;
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(*dict_, freqs, alphabet, min_probability, 1));
}
