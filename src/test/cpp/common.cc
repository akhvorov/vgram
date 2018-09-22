//
// Created by Aleksandr Khvorov on 02.09.18.
//

#include "common.h"

std::vector<std::vector<int>> simple_seqs() {
    std::vector<std::vector<int>> seqs;
    int seq1[] = {0, 1}; // 1
    int seq2[] = {0}; // 0
    int seq3[] = {1}; // 3
    int seq4[] = {0, 1, 1}; // 2
    seqs.push_back(std::vector<int>(std::begin(seq1), std::end(seq1)));
    seqs.push_back(std::vector<int>(std::begin(seq2), std::end(seq2)));
    seqs.push_back(std::vector<int>(std::begin(seq3), std::end(seq3)));
    seqs.push_back(std::vector<int>(std::begin(seq4), std::end(seq4)));
    return seqs;
}