//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#include <iostream>
#include <iomanip>
#include <utility>
#include "py_stream_vgram_builder.h"


PyStreamVGramBuilder::PyStreamVGramBuilder(int size)
        : PyStreamVGramBuilder(size, 1) {}

PyStreamVGramBuilder::PyStreamVGramBuilder(int size, int verbose = 1) :
        stream_builder_(std::make_shared<PyIntStreamVGramBuilder>(size, verbose)),
        tokenizer_(std::make_shared<CharTokenizer>()) {
}

PyStreamVGramBuilder::PyStreamVGramBuilder(std::shared_ptr<PyIntStreamVGramBuilder> stream_builder,
                                           std::shared_ptr<BaseTokenizer> tokenizer)
        : stream_builder_(std::move(stream_builder)), tokenizer_(std::move(tokenizer)) {
}

void PyStreamVGramBuilder::accept(const std::string &seq) {
    stream_builder_->accept(tokenizer_->transform(seq));
}

std::vector<std::string> PyStreamVGramBuilder::parse(const std::string &seq) const {
    IntSeq intSeq = tokenizer_->transform(seq);
    IntSeq tokens_ids = stream_builder_->parse(intSeq);
    std::vector<std::string> token_words;
    for (int token_id : tokens_ids) {
        std::vector<int> token_raw_representation = stream_builder_->get_dict()->alphabet()[token_id];
        std::vector<int> token_int_representation = stream_builder_->get_coder()->decode(token_raw_representation);
        token_words.push_back(tokenizer_->decode(token_int_representation));
    }
    return token_words;
}

void PyStreamVGramBuilder::update_dict() {
    stream_builder_->update_dict();
}

std::vector<std::string> PyStreamVGramBuilder::alphabet() const {
    return tokenizer_->decode(stream_builder_->alphabet());
}

IntSeq PyStreamVGramBuilder::freqs() const {
    return stream_builder_->freqs();
}

void PyStreamVGramBuilder::save(const std::string &filename) const {
    stream_builder_->save(filename, tokenizer_);
}
