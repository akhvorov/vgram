//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cpp/int_dict_p.h>
#include <cpp/int_vgram_builder_p.h>
#include "py_stream_vgram_builder.h"

PyStreamVGramBuilder::PyStreamVGramBuilder(int size) : PyStreamVGramBuilder(size, 1) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(int size, int verbose) : PyStreamVGramBuilder(size, "", verbose) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(int size, const std::string& filename)
: PyStreamVGramBuilder(size, filename, 1) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(int size, const std::string& filename, int verbose) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1, verbose));
    size_ = size;
    dict_ = nullptr;
    coder_ = SeqCoder();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    verbose_ = verbose;
    filename_ = filename;
}

PyStreamVGramBuilder::PyStreamVGramBuilder(const std::string& filename) : PyStreamVGramBuilder(filename, 1) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(const std::string& filename, int verbose) {
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

void PyStreamVGramBuilder::accept(const IntSeq& seq) {
    builder_->accept(coder_.encode_immutable(seq));
}

IntSeq PyStreamVGramBuilder::parse(const IntSeq& seq) const {
    IntSeq result;
    dict_->parse(coder_.encode_immutable(seq), freqs_, total_freqs_, &result);
    return result;
}

void PyStreamVGramBuilder::update_dict() {
    if (verbose_ > 0) {
        std::cout << "update dict" << std::endl;
    }
    if (dict_ != builder_->result()) {
        if (verbose_ > 0) {
            std::cout << "dict updated" << std::endl;
        }
        dict_ = builder_->result();
        builder_->result_freqs(&freqs_);
        total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
    }
}

void PyStreamVGramBuilder::save(const std::string& filename, BaseTokenizer* tokenizer) const {
    std::ofstream file;
    if (filename.empty() && !filename_.empty()) {
        file = std::ofstream(filename_);
    } else if (!filename.empty()) {
        file = std::ofstream(filename);
    } else {
        std::cout << "Error: no filename for save. Pass filename to constructor or save method" << std::endl;
    }
    file << std::setw(2) << dict_to_json(tokenizer) << std::endl;
    file.close();
}

json PyStreamVGramBuilder::dict_to_json(BaseTokenizer* tokenizer) const {
    json dict;
    dict["size"] = size_;
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
