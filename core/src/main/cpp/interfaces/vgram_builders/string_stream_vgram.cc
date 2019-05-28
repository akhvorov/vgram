//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#include <iostream>
#include <iomanip>
#include <utility>
#include "string_stream_vgram.h"

using namespace vgram_core;

StringStreamVGram::StringStreamVGram(int size) : StringStreamVGram(size, 1) {}

StringStreamVGram::StringStreamVGram(int size, int verbose = 1)
        : stream_builder_(std::make_shared<IntStreamVGram>(size, verbose)),
          tokenizer_(std::make_shared<CharTokenizer>()) {}

StringStreamVGram::StringStreamVGram(int size, BaseTokenizer *tokenizer)
        : stream_builder_(std::make_shared<IntStreamVGram>(size, 1)), tokenizer_(tokenizer) {}

StringStreamVGram::StringStreamVGram(int size, BaseTokenizer *tokenizer, int verbose = 1)
        : stream_builder_(std::make_shared<IntStreamVGram>(size, verbose)), tokenizer_(tokenizer) {}

StringStreamVGram::StringStreamVGram(std::shared_ptr<IntStreamVGram> stream_builder,
                                     std::shared_ptr<BaseTokenizer> tokenizer)
        : stream_builder_(std::move(stream_builder)), tokenizer_(std::move(tokenizer)) {
}

void StringStreamVGram::accept(const std::string &seq) {
    stream_builder_->accept(tokenizer_->transform(seq));
}

std::vector<std::string> StringStreamVGram::parse(const std::string &seq) const {
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

void StringStreamVGram::update_dict() {
    stream_builder_->update_dict();
}

std::vector<std::string> StringStreamVGram::alphabet() const {
    return tokenizer_->decode(stream_builder_->alphabet());
}

IntSeq StringStreamVGram::freqs() const {
    return stream_builder_->freqs();
}

void StringStreamVGram::save(const std::string &filename) const {
    stream_builder_->save(filename, tokenizer_);
}
