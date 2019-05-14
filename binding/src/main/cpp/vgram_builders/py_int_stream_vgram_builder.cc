//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <utility>
#include <src/main/cpp/int_dict_p.h>
#include <src/main/cpp/int_vgram_builder_p.h>
#include "py_int_stream_vgram_builder.h"


PyIntStreamVGramBuilder::PyIntStreamVGramBuilder(int size)
        : PyIntStreamVGramBuilder(size, 1) {}

PyIntStreamVGramBuilder::PyIntStreamVGramBuilder(int size, int verbose = 1) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1, verbose));
    size_ = size;
    dict_ = nullptr;
    coder_ = std::make_shared<SeqCoder>();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    verbose_ = verbose;
}

void PyIntStreamVGramBuilder::accept(const IntSeq &seq) {
    builder_->accept(coder_->encode(seq));
}

IntSeq PyIntStreamVGramBuilder::parse(const IntSeq &seq) const {
//    std::cout << "in parse" << std::endl;
    IntSeq result;
//    if (dict_) {
//        std::cout << "dict is true" << std::endl;
//    }
//    std::cout << "after if" << std::endl;
    coder_->encode_immutable(seq);
//    std::cout << "before coder call" << std::endl;
    builder_->alpha();
//    std::cout << "before builder call" << std::endl;
//    std::cout << "Size " << dict_->size() << "!" << std::endl;
//    std::cout << "before parse" << std::endl;
    dict_->parse(coder_->encode_immutable(seq), freqs_, total_freqs_, &result);
//    std::cout << "out parse" << std::endl;
    return result;
}

void PyIntStreamVGramBuilder::update_dict() {
    if (dict_ != builder_->result()) {
        dict_ = builder_->result();
        builder_->result_freqs(&freqs_);
        total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
        min_probability_ = builder_->get_min_probability();
    }
}

std::vector<IntSeq> PyIntStreamVGramBuilder::alphabet() const {
    std::vector<IntSeq> alpha;
    for (const auto &a : dict_->alphabet()) {
        alpha.push_back(coder_->decode(a));
    }
    return alpha;
}

IntSeq PyIntStreamVGramBuilder::freqs() const {
    return freqs_;
}

void PyIntStreamVGramBuilder::save(const std::string &filename, std::shared_ptr<BaseTokenizer> tokenizer) const {
    std::ofstream file;
    file = std::ofstream(filename);
//    std::cout << "in save" << std::endl;
    file << std::setw(2) << dict_to_json(std::move(tokenizer)) << std::endl;
    file.close();
}

std::shared_ptr<SeqCoder> PyIntStreamVGramBuilder::get_coder() {
    return coder_;
}

std::shared_ptr<IntDict> PyIntStreamVGramBuilder::get_dict() {
    return dict_;
}

json PyIntStreamVGramBuilder::dict_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const {
    json dict;
    dict["size"] = size_;
    dict["min_prob"] = min_probability_;
    dict["coder"] = coder_to_json();
    dict["alphabet"] = alphabet_to_json(std::move(tokenizer));
    return dict;
}

json PyIntStreamVGramBuilder::coder_to_json() const {
    std::vector<std::pair<int, int>> pairs;
    for (const auto &p : coder_->code_map()) {
        pairs.emplace_back(p);
    }
    std::sort(pairs.begin(), pairs.end(),
              [](const std::pair<int, int> &a, const std::pair<int, int> &b) { return a.second < b.second; });
    std::string res;
    json coder;
    for (const auto &p : pairs) {
        coder.push_back(p.first);
    }
    return coder;
}

json PyIntStreamVGramBuilder::alphabet_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const {
    json alpha;
    std::vector<IntSeq> alphabet = dict_->alphabet();
    for (int i = 0; i < alphabet.size(); i++) {
        IntSeq word = alphabet[i];
        json word_obj;
        word_obj["vec"] = json(word);
        word_obj["freq"] = freqs_[i];
        if (tokenizer != nullptr)
            word_obj["text"] = tokenizer->decode(coder_->decode(dict_->get(i)));
        alpha.push_back(word_obj);
    }
    return alpha;
}

PyIntStreamVGramBuilder::PyIntStreamVGramBuilder(const std::shared_ptr<SeqCoder> coder, const IntSeq &freqs,
                                                 const std::vector<IntSeq> &alphabet, int size,
                                                 double min_probability) {
//    std::cout << "in ctor" << std::endl;
    size_ = size;
    min_probability_ = min_probability;
    freqs_ = freqs;
    total_freqs_ = std::accumulate(freqs.begin(), freqs.end(), 0);
    coder_ = coder;
    dict_ = std::make_shared<IntDictImpl>(alphabet);
//    std::cout << "dict_ inited" << std::endl;
//    std::cout << "Size " << dict_->size() << "!" << std::endl;
    builder_ = std::make_shared<IntVGramBuilderImpl>(*dict_, freqs, alphabet, min_probability, 1);
//    std::cout << "out ctor" << std::endl;
}
