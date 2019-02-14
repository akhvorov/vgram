//
// Created by Aleksandr Khvorov on 17/10/2018.
//

#ifndef DICT_EXPANSION_SEQ_CODER_H
#define DICT_EXPANSION_SEQ_CODER_H

#include <unordered_map>

template <typename T>
class SeqCoder {
public:
    SeqCoder() {
        forward_code_ = std::unordered_map<T, int>();
        backward_code_ = std::unordered_map<int, T>();
    }

    std::vector<int> encode(const std::vector<T>& seq);

    std::vector<int> encode_immutable(const std::vector<T>& seq) const;

//    void emplace_encode(std::vector<T>& seq);
//    void emplace_encode_immutable(std::vector<T>& seq) const;
    std::vector<T> decode(const std::vector<int>& seq) const;
//    void emplace_decode(std::vector<int>& seq) const;
    const std::unordered_map<T, int> code_map() const;

private:
    std::unordered_map<T, int> forward_code_;
    std::unordered_map<int, T> backward_code_;
};


#endif //DICT_EXPANSION_SEQ_CODER_H
