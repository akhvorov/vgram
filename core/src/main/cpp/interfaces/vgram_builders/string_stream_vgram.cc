//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#include <iostream>
#include <iomanip>
#include <utility>
#include <saver.h>
#include "string_stream_vgram.h"

using namespace std;
using namespace vgram_core;

StringStreamVGram::StringStreamVGram(int size, int verbose, bool inner_words_split, bool words_are_tokens)
        : stream_builder_(make_shared<IntStreamVGram>(size, verbose)),
          inner_words_split(inner_words_split), words_are_tokens(words_are_tokens) {
    check_args();
    if (words_are_tokens) {
        tokenizer_ = make_shared<WordTokenizer>();
    } else {
        tokenizer_ = make_shared<CharTokenizer>();
    }
//    if (inner_words_split) {
//        splitter_ = std::make_shared<WordSplitter>();
//    } else {
//        splitter_ = std::make_shared<NoSplitter>();
//    }
}

StringStreamVGram::StringStreamVGram(shared_ptr<IntStreamVGram> stream_builder,
                                     shared_ptr<BaseTokenizer> tokenizer,
                                     bool inner_words_split, bool words_are_tokens)
        : stream_builder_(std::move(stream_builder)), tokenizer_(std::move(tokenizer)),
          inner_words_split(inner_words_split), words_are_tokens(words_are_tokens) {
    check_args();
}

void StringStreamVGram::accept(const string &seq) {
    stream_builder_->accept(tokenizer_->transform(seq));
}

vector<string> StringStreamVGram::parse(const string &seq) const {
    IntSeq intSeq = tokenizer_->transform(seq);
    IntSeq tokens_ids = stream_builder_->parse(intSeq);
    vector<string> token_words;
    for (int token_id : tokens_ids) {
        vector<int> token_raw_representation = stream_builder_->get_dict()->alphabet()[token_id];
        vector<int> token_int_representation = stream_builder_->get_coder()->decode(token_raw_representation);
        token_words.push_back(tokenizer_->decode(token_int_representation));
    }
    return token_words;
}

void StringStreamVGram::update_dict() {
    stream_builder_->update_dict();
}

vector<string> StringStreamVGram::alphabet() const {
    return tokenizer_->decode(stream_builder_->alphabet());
}

IntSeq StringStreamVGram::freqs() const {
    return stream_builder_->freqs();
}

void StringStreamVGram::save(const string &filename) const {
    Saver::save(this, filename);
}

void StringStreamVGram::check_args() {
    if (words_are_tokens && inner_words_split) {
        throw invalid_argument("Inner_words_split and words_are_tokens can't be true both");
    }
}
