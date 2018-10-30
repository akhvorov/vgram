//
// Created by Aleksandr Khvorov on 02.09.18.
//

#include "common.h"

std::vector<std::vector<int>> simple_seqs() {
    std::vector<std::vector<int>> seqs;
    seqs.push_back(std::vector<int>{0, 1});
    seqs.push_back(std::vector<int>{0});
    seqs.push_back(std::vector<int>{1});
    seqs.push_back(std::vector<int>{0, 1, 1});
    return seqs;
}
