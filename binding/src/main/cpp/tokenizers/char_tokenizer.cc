//
// Created by Aleksandr Khvorov on 19/10/2018.
//

#import <algorithm>
#import <string>
#include "char_tokenizer.h"

CharTokenizer::CharTokenizer() = default;

CharTokenizer::CharTokenizer(const std::unordered_map<std::string, int> &forward_map) : BaseTokenizer(forward_map) {}

std::string CharTokenizer::normalize(const std::string &str) const {
    std::string res;
    for (char i : str) {
        if (isalnum(i)) {
            res += i;
        }
    }
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
    // return regex_replace(str, std::regex("[^\\w\\d]"), "");
}

std::vector<std::string> CharTokenizer::tokenize(const std::string &str) const {
    std::vector<std::string> tokens;
    for (char i : str) {
        tokens.emplace_back(1, i);
    }
    return tokens;
}