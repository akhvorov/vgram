//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_INTDICTIMPL_H
#define DICT_EXPANSION_INTDICTIMPL_H

#include "int_dict.h"
#include <vector>
#include <unordered_set>

class IntDictImpl : public IntDict {
    int linearParse(const std::vector<int>& seq, std::vector<int>& builder,
                    const std::unordered_set<int>& excludes = nullptr);

    double weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double totalFreq,
                         std::vector<int>& builder, const std::unordered_set<int>& excludes = nullptr);

public:
    int search(const std::vector<int>& seq, const std::vector<int>& excludes = nullptr) const;

    std::vector<int> parse(std::vector<int> seq, std::vector<int> freqs, double totalFreq);
    std::vector<int> parse(std::vector<int> seq, std::unordered_set <int> excludes = nullptr);

    std::vector<int> get(int index) const;

    int size() const;

    std::vector<std::vector<int>> alphabet() const;

    int parent(int second) const;

    ~IntDictImpl();
};


#endif //DICT_EXPANSION_INTDICTIMPL_H
