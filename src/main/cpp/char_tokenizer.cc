//
// Created by Aleksandr Khvorov on 19/10/2018.
//

//#import <regex>
#include <algorithm>
#include <string>
#include "char_tokenizer.h"

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