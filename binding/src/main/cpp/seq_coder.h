//
// Created by Aleksandr Khvorov on 17/10/2018.
//

#ifndef DICT_EXPANSION_SEQ_CODER_H
#define DICT_EXPANSION_SEQ_CODER_H

#include <unordered_map>

class SeqCoder {
public:
    SeqCoder();
    std::vector<int> encode(const std::vector<int>& seq);
    std::vector<int> encode_immutable(const std::vector<int>& seq) const;
    std::vector<int> decode(const std::vector<int>& seq) const;
    const std::unordered_map<int, int> code_map() const;

private:
    std::unordered_map<int, int> forward_code_;
    std::unordered_map<int, int> backward_code_;
};


#endif //DICT_EXPANSION_SEQ_CODER_H
