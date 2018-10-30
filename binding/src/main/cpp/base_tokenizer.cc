//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#include <sstream>
#import <regex>
#include "base_tokenizer.h"

BaseTokenizer* BaseTokenizer::fit(const std::vector<std::string>& seqs, py::args args) {
    int size = 0;
    for (const std::string& seq : seqs) {
        for (const auto& symb : tokenize(normalize(seq))) {
            if (forward_coder_.count(symb) == 0) {
                forward_coder_[symb] = size;
                backward_coder_[size] = symb;
                size++;
            }
        }
    }
    return this;
}

std::vector<std::vector<int>> BaseTokenizer::transform(const std::vector<std::string>& seqs, py::args args) const {
    std::vector<std::vector<int>> res;
    res.reserve(seqs.size());
    for (const std::string& seq : seqs) {
        std::vector<std::string> tokens = tokenize(normalize(seq));
        std::vector<int> coded_seq;
        coded_seq.reserve(tokens.size());
        for (const std::string& symb : tokens) {
            coded_seq.push_back(forward_coder_.at(symb));
        }
        res.push_back(coded_seq);
    }
    return res;
}

std::string BaseTokenizer::normalize(const std::string& s) const {
    return s;
}

std::vector<std::string> BaseTokenizer::tokenize(const std::string& str) const {
    std::vector<std::string> tokens;
    for (char i : str) {
        tokens.emplace_back(1, i);
    }
    return tokens;
}

std::vector<std::string> BaseTokenizer::decode(const std::vector<std::vector<int>>& seqs) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const std::vector<int>& seq : seqs) {
        std::string decoded_seq;
        for (auto i : seq) {
            if (backward_coder_.count(i) > 0) {
                decoded_seq += backward_coder_.at(i);
            }
        }
        res.push_back(decoded_seq);
    }
    return res;
}
