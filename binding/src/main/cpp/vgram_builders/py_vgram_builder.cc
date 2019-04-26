
//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#include <utility>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) : PyVGramBuilder(size, iter_num, 1) {}

PyVGramBuilder::PyVGramBuilder(int size, int iter_num, int verbose)
        : int_builder_(std::make_shared<PyIntVGramBuilder>(size, iter_num, verbose)),
          tokenizer_(std::make_shared<CharTokenizer>()) {}

PyVGramBuilder *PyVGramBuilder::fit(const std::vector<std::string> &seqs, py::args &args) {
    int_builder_->fit(tokenizer_->transform(seqs, args), args);
    return this;
}

std::vector<std::string> PyVGramBuilder::transform(const std::vector<std::string> &seqs, py::args &args) const {
    std::vector<IntSeq> coded_texts_by_token_ids = int_builder_->transform(tokenizer_->transform(seqs, args));
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

void PyVGramBuilder::save(const std::string &filename) const {
    int_builder_->save(filename, tokenizer_);
}

IntSeq PyVGramBuilder::freqs() const {
    return int_builder_->freqs();
}

std::vector<std::string> PyVGramBuilder::alphabet() const {
    return tokenizer_->decode(int_builder_->alphabet());
}

PyVGramBuilder::PyVGramBuilder(std::shared_ptr<PyIntVGramBuilder> int_builder, std::shared_ptr<BaseTokenizer> tokenizer)
        : int_builder_(std::move(int_builder)), tokenizer_(std::move(tokenizer)) {}
