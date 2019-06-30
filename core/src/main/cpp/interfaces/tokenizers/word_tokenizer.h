//
// Created by Aleksandr Khvorov on 30/06/2019.
//

#ifndef DICT_EXPANSION_WORDTOKENIZER_H
#define DICT_EXPANSION_WORDTOKENIZER_H


#include "base_tokenizer.h"

namespace vgram_core {
    class WordTokenizer : public BaseTokenizer {
        WordTokenizer();

        explicit WordTokenizer(const std::unordered_map<std::string, int> &forward_map);

        std::string normalize(const std::string &str) const override;

        std::vector<std::string> tokenize(const std::string &str) const override;
    };
}


#endif //DICT_EXPANSION_WORDTOKENIZER_H
