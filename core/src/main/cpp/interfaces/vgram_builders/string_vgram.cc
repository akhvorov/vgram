//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#include <utility>
#include "string_vgram.h"

using namespace vgram_core;

StringVGram::StringVGram(int size, int iter_num) : StringVGram(size, iter_num, 1) {}

StringVGram::StringVGram(int size, int iter_num, int verbose)
        : int_builder_(std::make_shared<IntVGram>(size, iter_num, verbose)),
          tokenizer_(std::make_shared<CharTokenizer>()) {}

StringVGram::StringVGram(int size, int iter_num, BaseTokenizer *tokenizer)
        : int_builder_(std::make_shared<IntVGram>(size, iter_num, 1)),
          tokenizer_(tokenizer) {}

StringVGram::StringVGram(int size, int iter_num, BaseTokenizer *tokenizer, int verbose = 1)
        : int_builder_(std::make_shared<IntVGram>(size, iter_num, verbose)),
          tokenizer_(tokenizer) {}

StringVGram *StringVGram::fit(const std::vector<std::string> &seqs) {
    int_builder_->fit(tokenizer_->transform(seqs));
    return this;
}

std::vector<std::string> StringVGram::transform(const std::vector<std::string> &seqs) const {
    std::vector<IntSeq> coded_texts_by_token_ids = int_builder_->transform(tokenizer_->transform(seqs));
    std::vector<std::string> decoded_texts;
    std::vector<IntSeq> alphabet = int_builder_->alphabet();
    for (const IntSeq &coded_text_by_token_ids : coded_texts_by_token_ids) {
        std::string decoded_words;
        for (int word_id : coded_text_by_token_ids) {
            if (!decoded_words.empty()) {
                decoded_words += " ";
            }
            decoded_words += tokenizer_->decode(alphabet[word_id]);
        }
        decoded_texts.push_back(decoded_words);
    }
    return decoded_texts;
}

void StringVGram::save(const std::string &filename) const {
    int_builder_->save(filename, tokenizer_);
}

IntSeq StringVGram::freqs() const {
    return int_builder_->freqs();
}

std::vector<std::string> StringVGram::alphabet() const {
    return tokenizer_->decode(int_builder_->alphabet());
}

StringVGram::StringVGram(std::shared_ptr<IntVGram> int_builder, std::shared_ptr<BaseTokenizer> tokenizer)
        : int_builder_(std::move(int_builder)), tokenizer_(std::move(tokenizer)) {}
