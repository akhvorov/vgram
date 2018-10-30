//
// Created by Aleksandr Khvorov on 02.09.18.
//

#ifndef DICT_EXPANSION_VECTOR_HASH_H
#define DICT_EXPANSION_VECTOR_HASH_H

#import <vector>
#include <unordered_set>

struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

#endif //DICT_EXPANSION_VECTOR_HASH_H
