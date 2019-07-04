//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#include <algorithm>
#include <iomanip>
#include <utility>
#include "../../algorithm/int_dict_p.h"
#include "../../algorithm/int_vgram_builder_p.h"
#include "int_stream_vgram.h"
#include "../saver.h"

using namespace vgram_core;

IntStreamVGram::IntStreamVGram(int size)
        : IntStreamVGram(size, 1) {}

IntStreamVGram::IntStreamVGram(int size, int verbose = 1) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1, verbose));
    size_ = size;
    dict_ = nullptr;
    coder_ = std::make_shared<SeqCoder>();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    verbose_ = verbose;
}

void IntStreamVGram::accept(const IntSeq &seq) {
    builder_->accept(coder_->encode(seq));
}

IntSeq IntStreamVGram::parse(const IntSeq &seq) const {
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

void IntStreamVGram::update_dict() {
    if (dict_ != builder_->result()) {
        dict_ = builder_->result();
        builder_->result_freqs(&freqs_);
        total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
        min_probability_ = builder_->get_min_probability();
    }
}

std::vector<IntSeq> IntStreamVGram::alphabet() const {
    std::vector<IntSeq> alpha;
    for (const auto &a : dict_->alphabet()) {
        alpha.push_back(coder_->decode(a));
    }
    return alpha;
}

IntSeq IntStreamVGram::freqs() const {
    return freqs_;
}

void IntStreamVGram::save(const std::string &filename, std::shared_ptr<BaseTokenizer> tokenizer) const {
    Saver::save(this, filename);
}

const std::shared_ptr<SeqCoder> IntStreamVGram::get_coder() const {
    return coder_;
}

std::shared_ptr<IntDict> IntStreamVGram::get_dict() {
    return dict_;
}

IntStreamVGram::IntStreamVGram(const std::shared_ptr<SeqCoder> coder, const IntSeq &freqs,
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
