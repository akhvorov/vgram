//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#ifndef DICT_EXPANSION_SIMPLE_TOKENIZER_H
#define DICT_EXPANSION_SIMPLE_TOKENIZER_H

#import "base_tokenizer.h"

class CharTokenizer : public BaseTokenizer {
public:
    CharTokenizer();

    explicit CharTokenizer(const std::unordered_map<std::string, int> &forward_map);

    std::string normalize(const std::string &str) const override;

    std::vector<std::string> tokenize(const std::string &str) const override;
};

#endif //DICT_EXPANSION_SIMPLE_TOKENIZER_H
