//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#ifndef VGRAM_INTERFACES_STRING_VGRAM_H
#define VGRAM_INTERFACES_STRING_VGRAM_H

#include "../tokenizers/char_tokenizer.h"
#include "int_vgram.h"
#include "string_stream_vgram.h"

namespace vgram_core {
    class StringVGram {
    public:
        static StringVGram *load(const std::string &filename) {
            std::shared_ptr<IntVGram> int_builder = std::shared_ptr<IntVGram>(IntVGram::load(filename));
            std::shared_ptr<BaseTokenizer> tokenizer =
                    StringStreamVGram::loadTokenizer(filename, int_builder->get_coder());
//            return std::make_shared<StringVGram>(int_builder, tokenizer);
            return new StringVGram(int_builder, tokenizer);
        }

        StringVGram(int size, int iter_num);

        StringVGram(int size, int iter_num, int verbose);

        StringVGram(int size, int iter_num, BaseTokenizer *tokenizer);

        StringVGram(int size, int iter_num, BaseTokenizer *tokenizer, int verbose);

        StringVGram(std::shared_ptr<IntVGram> int_builder, std::shared_ptr<BaseTokenizer> tokenizer);

        virtual ~StringVGram() = default;

        StringVGram *fit(const std::vector<std::string> &seqs/*, const std::string &filename,*/);

        std::vector<std::string> transform(const std::vector<std::string> &seqs) const;

        void save(const std::string &filename) const;

        IntSeq freqs() const;

        std::vector<std::string> alphabet() const;

    private:
        std::shared_ptr<IntVGram> int_builder_;
        std::shared_ptr<BaseTokenizer> tokenizer_;
    };
}

#endif //VGRAM_INTERFACES_STRING_VGRAM_H
