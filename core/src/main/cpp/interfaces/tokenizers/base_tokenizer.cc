//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#include <sstream>
#import <regex>
#include "base_tokenizer.h"

using namespace std;
using namespace vgram_core;

const std::string BaseTokenizer::kSpaceReplacer = "\u2581";

BaseTokenizer::BaseTokenizer(const unordered_map<string, int> &forward_map) {
    forward_coder_ = forward_map;
    for (const auto &pair : forward_map) {
        backward_coder_[pair.second] = pair.first;
    }
}

vector<vector<int>> BaseTokenizer::transform(const vector<string> &seqs) {
    std::vector<std::vector<int>> res;
    res.reserve(seqs.size());
    for (const std::string &seq : seqs) {
        res.push_back(transform(seq));
    }
    return res;
}

vector<int> BaseTokenizer::transform(const string &seq) {
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

string BaseTokenizer::normalize(const string &str) const {
    return str;
}

vector<string> BaseTokenizer::decode(const vector<vector<int>> &seqs) const {
    vector<string> res;
    res.reserve(seqs.size());
    for (auto &seq : seqs) {
        res.push_back(decode(seq));
    }
    return res;
}

string BaseTokenizer::decode(const vector<int> &seq) const {
    string decoded_seq;
    for (auto i : seq) {
        if (backward_coder_.count(i) > 0) {
            decoded_seq += backward_coder_.at(i);
        }
    }
    return decoded_seq;
}
