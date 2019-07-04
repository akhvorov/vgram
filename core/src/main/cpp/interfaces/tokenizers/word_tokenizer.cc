//
// Created by Aleksandr Khvorov on 30/06/2019.
//

#include "word_tokenizer.h"

using namespace std;
using namespace vgram_core;

WordTokenizer::WordTokenizer() = default;

WordTokenizer::WordTokenizer(const unordered_map<string, int> &forward_map) : BaseTokenizer(forward_map) {}

string WordTokenizer::normalize(const string &str) const {
    return str;
}

vector<string> WordTokenizer::tokenize(const string &str) const {
    vector<string> tokens;
    string current;
    for (char i : str) {
        if (i == ' ' && current != "") {
            tokens.push_back(current);
            current = "";
        } else {
            current += i;
        }
    }
    if (current != "") {
        tokens.push_back(current);
    }
    return tokens;
}
