//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#include <sstream>
#import <regex>
#include "tokenizer.h"

Tokenizer::Tokenizer() {
    sep_ = ' ';
}

std::vector<int> Tokenizer::tokenize(const std::string& str) {
    //std::cout << "tokenize!" << std::endl;
    std::vector<int> res;
    for (char i : str) {
        if (dict_.count(i) == 0) {
            dict_[i] = static_cast<int>(dict_.size());
        }
        res.push_back(dict_[i]);
    }
//    std::string symbol;
//    std::stringstream stream(str);
//    size_t pos = 0;
//    while ((pos = str.find(sep_)) != std::string::npos) {
//        symbol = str.substr(0, pos);
//        if (dict_.count(symbol) == 0) {
//            dict_[symbol] = static_cast<int>(dict_.size());
//        }
//        res.push_back(dict_[symbol]);
//    }
    return res;
}

std::string Tokenizer::normalize(const std::string& str) {
    //std::cout << "normalize!" << std::endl;
    return std::regex_replace(str, std::regex("[^\\pL\\pN]"), "");
}

void Tokenizer::fit(const std::vector<std::string>& seqs) {//, const std::vector<int>& target) {
//        for (const std::string& seq : seqs) {
//            tokenize(normalize(seq));
//        }
}

std::vector<std::vector<int>> Tokenizer::transform(const std::vector<std::string>& seqs) { //, const std::vector<int>& target) {
    std::vector<std::vector<int>> res;
    res.reserve(seqs.size());
    for (const std::string& seq : seqs) {
        //std::cout << "loop!" << std::endl;
        //res.push_back(tokenize(normalize(seq)));
        res.push_back(tokenize(seq));
    }
    return res;
}
