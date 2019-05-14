//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#include <sstream>
#import <regex>
#include "base_tokenizer.h"


BaseTokenizer::BaseTokenizer(const std::unordered_map<std::string, int> &forward_map) {
    forward_coder_ = forward_map;
    for (const auto &pair : forward_map) {
        backward_coder_[pair.second] = pair.first;
    }
}

BaseTokenizer *BaseTokenizer::fit(const std::vector<std::string> &seqs, py::args args) {
    return this;
}

std::vector<std::vector<int>> BaseTokenizer::transform(const std::vector<std::string> &seqs, py::args &args) {
    std::vector<std::vector<int>> res;
    res.reserve(seqs.size());
    for (const std::string &seq : seqs) {
        res.push_back(transform(seq));
    }
    return res;
}

std::vector<int> BaseTokenizer::transform(const std::string &seq) {
    std::vector<std::string> tokens = tokenize(normalize(seq));
    std::vector<int> coded_seq;
    coded_seq.reserve(tokens.size());
    for (const std::string &symb : tokens) {
        if (forward_coder_.count(symb) == 0) {
            forward_coder_[symb] = static_cast<int>(forward_coder_.size());
            backward_coder_[backward_coder_.size()] = symb;
        }
        coded_seq.push_back(forward_coder_.at(symb));
    }
    return coded_seq;
}

std::vector<std::vector<int>> BaseTokenizer::fit_transform(const std::vector<std::string> &seqs, py::args &args) {
    return transform(seqs, args);
}

std::string BaseTokenizer::normalize(const std::string &str) const {
//    std::string res;
//    for (char symb : str) {
//        if (symb == ' ') {
////            res += "\u2581";
//            res += '_';
//        } else {
//            res += symb;
//        }
//    }
    return str;
}

std::vector<std::string> BaseTokenizer::tokenize(const std::string &str) const {
    std::vector<std::string> tokens;
    for (char i : str) {
        tokens.emplace_back(1, i);
    }
    return tokens;
}

std::vector<std::string> BaseTokenizer::decode(const std::vector<std::vector<int>> &seqs) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const std::vector<int> &seq : seqs) {
        res.push_back(decode(seq));
    }
    return res;
}

std::string BaseTokenizer::decode(const std::vector<int> &seq) const {
    std::string decoded_seq;
    for (auto i : seq) {
        if (backward_coder_.count(i) > 0) {
            decoded_seq += backward_coder_.at(i);
        }
    }
    return decoded_seq;
}
