//
// Created by Aleksandr Khvorov on 19/10/2018.
//

#import <algorithm>
#import <string>
#include "char_tokenizer.h"

using namespace std;
using namespace vgram_core;

CharTokenizer::CharTokenizer() = default;

CharTokenizer::CharTokenizer(const unordered_map<string, int> &forward_map) : BaseTokenizer(forward_map) {}

string CharTokenizer::normalize(const string &str) const {
    string res;
    for (auto &c : str) {
        if (c == ' ') {
            res += BaseTokenizer::kSpace;
        } else {
            res += c;
        }
    }
    return res;
}

vector<string> CharTokenizer::tokenize(const string &str) const {
    vector<string> tokens;
    for (char i : str) {
        tokens.emplace_back(1, i);
    }
    return tokens;
}
