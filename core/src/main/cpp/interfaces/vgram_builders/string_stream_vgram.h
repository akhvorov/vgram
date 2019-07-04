//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#ifndef VGRAM_INTERFACES_STRING_STREAM_VGRAM_H
#define VGRAM_INTERFACES_STRING_STREAM_VGRAM_H

#include "../tokenizers/char_tokenizer.h"
#include "../tokenizers/word_tokenizer.h"
#include "../word_splitter/word_splitter.h"
#include "int_stream_vgram.h"

using json = nlohmann::json;

namespace vgram_core {
    class Saver;

    class StringStreamVGram {
        friend class Saver;
    public:
        explicit StringStreamVGram(int size, int verbose = 1, bool inner_words_split = false, bool words_are_tokens = false);

        StringStreamVGram(std::shared_ptr<IntStreamVGram> stream_builder,
                          std::shared_ptr<BaseTokenizer> tokenizer,
                          bool inner_words_split = false, bool words_are_tokens = false);

        void accept(const std::string &seq);

        std::vector<std::string> parse(const std::string &seq) const;

        void save(const std::string &filename) const;

        std::vector<std::string> alphabet() const;

        IntSeq freqs() const;

        void update_dict();

    protected:
        std::shared_ptr<IntStreamVGram> stream_builder_;
        std::shared_ptr<BaseTokenizer> tokenizer_;
//        std::shared_ptr<PieceSplitter> splitter_;
        bool words_are_tokens = false;
        bool inner_words_split = false;

        void check_args();
    };
}

#endif //VGRAM_INTERFACES_STRING_STREAM_VGRAM_H
