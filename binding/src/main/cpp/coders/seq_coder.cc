//
// Created by Aleksandr Khvorov on 17/10/2018.
//

#include <vector>
#include <string>
#include <algorithm>
#include "seq_coder.h"

template <typename T>
std::vector<int> SeqCoder<T>::encode(const std::vector<T>& seq) {
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

template <typename T>
std::vector<int> SeqCoder<T>::encode_immutable(const std::vector<T>& seq) const {
    std::vector<int> result;
    for (int symbol : seq) {
        if (forward_code_.count(symbol) != 0) {
            result.push_back(forward_code_.at(symbol));
        }
    }
    return result;
}

//void SeqCoder::emplace_encode(std::vector<int>& seq) {
//    for (int &i : seq) {
//        if (forward_code_.count(i) == 0) {
//            int size = static_cast<int>(forward_code_.size());
//            forward_code_[i] = size;
//            backward_code_[size] = i;
//        }
//        i = forward_code_[i];
//    }
//}

template <typename T>
std::vector<T> SeqCoder<T>::decode(const std::vector<int>& seq) const {
    std::vector<T> result;
    result.reserve(seq.size());
    for (int symbol : seq) {
        result.push_back(backward_code_.at(symbol));
    }
    return result;
}

//void SeqCoder::emplace_encode_immutable(std::vector<int>& seq) const {
//    for (int &i : seq) {
//        if (forward_code_.count(i) != 0) {
//            i = forward_code_.at(i);
//        }
//    }
//}
//
//void SeqCoder::emplace_decode(std::vector<int>& seq) const {
//    for (int &i : seq) {
//        i = backward_code_.at(i);
//    }
//}

template <typename T>
const std::unordered_map<T, int> SeqCoder<T>::code_map() const {
    return forward_code_;
}
