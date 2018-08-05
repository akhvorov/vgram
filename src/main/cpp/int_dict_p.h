//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_INTDICTIMPL_H
#define DICT_EXPANSION_INTDICTIMPL_H

#include "int_dict.h"
#include <vector>
#include <unordered_set>

class IntDictImpl : public IntDict {
    int linearParse(std::vector<int> const & seq, std::vector<int> & builder,
                            std::unordered_set<int> const & excludes);

    double weightedParse(std::vector<int> const & seq, std::vector<int> const & freqs, double totalFreq,
                         std::vector<int> & builder, std::unordered_set<int> const & excludes);

public:
    int search(std::vector<int> seq, std::vector<int> excludes = nullptr);

    std::vector<int> parse(std::vector<int> seq, std::vector<int> freqs, double totalFreq);
    std::vector<int> parse(std::vector<int> seq, std::unordered_set <int> excludes = nullptr);

    std::vector<int> get(int index);

    int size();

    std::vector<std::vector<int>> alphabet();

    int parent(int second);

    ~IntDictImpl();
};


#endif //DICT_EXPANSION_INTDICTIMPL_H
