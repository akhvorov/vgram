//
// Created by Aleksandr Khvorov on 17/10/2018.
//

#include <vector>
#include <string>
#include <algorithm>
#include "seq_coder.h"

SeqCoder::SeqCoder() {
    forward_code_ = std::unordered_map<int, int>();
    backward_code_ = std::unordered_map<int, int>();
}

std::vector<int> SeqCoder::encode(const std::vector<int>& seq) {
    std::vector<int> result;
    for (int symbol : seq) {
        if (forward_code_.count(symbol) == 0) {
            int size = static_cast<int>(forward_code_.size());
            forward_code_[symbol] = size;
            backward_code_[size] = symbol;
        }
        result.push_back(forward_code_[symbol]);
    }
    return result;
}

std::vector<int> SeqCoder::encode(const std::vector<int>& seq) const {
    std::vector<int> result;
    for (int symbol : seq) {
        if (forward_code_.count(symbol) != 0) {
            result.push_back(forward_code_.at(symbol));
        }
    }
    return result;
}

void SeqCoder::emplace_encode(std::vector<int>& seq) {
    for (int &i : seq) {
        if (forward_code_.count(i) == 0) {
            int size = static_cast<int>(forward_code_.size());
            forward_code_[i] = size;
            backward_code_[size] = i;
        }
        i = forward_code_[i];
    }
}

void SeqCoder::emplace_encode(std::vector<int>& seq) const {
    for (int &i : seq) {
        if (forward_code_.count(i) != 0) {
            i = forward_code_.at(i);
        }
    }
}

std::vector<int> SeqCoder::decode(const std::vector<int>& seq) const {
    std::vector<int> result;
    result.reserve(seq.size());
    for (int symbol : seq) {
        result.push_back(backward_code_.at(symbol));
    }
    return result;
}

void SeqCoder::emplace_decode(std::vector<int>& seq) const {
    for (int &i : seq) {
        i = backward_code_.at(i);
    }
}

std::string SeqCoder::to_string() const {
    std::vector<std::pair<int, int>> pairs;
    for (const auto& p : forward_code_) {
        pairs.emplace_back(p);
    }
    std::sort(pairs.begin(), pairs.end(), [](const std::pair<int,int>& a, const std::pair<int,int>& b) { return a.second < b.second; });
    std::string res;
    for (const auto& p : pairs) {
        res += ":" + std::to_string(p.first) + "\n";
    }
    return res;
}
