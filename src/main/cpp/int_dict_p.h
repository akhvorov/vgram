//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_INTDICTIMPL_H
#define DICT_EXPANSION_INTDICTIMPL_H

#include "int_dict.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

class IntDictImpl : public IntDict {
public:
    IntDictImpl(const std::vector<std::vector<int>>& seqs);
    int search(const std::vector<int>& seq, const std::vector<int>& excludes = nullptr) const;
    std::vector<int> get(int index) const;
    int size() const;
    std::vector<std::vector<int>> alphabet() const;
    int parent(int second) const;

    int parse(const std::vector<int>& seq, std::vector<int>* output, const std::vector<int>& freqs);
    int parse(const std::vector<int>& seq, std::vector<int>* output, const std::unordered_set<int>& excludes = nullptr);
    void weightParseVariants(const std::vector<int>& seq, double multiplier, const std::vector<int>& freqs,
                             const std::unordered_set<int>& excludes, std::unordered_map<int, double>& result);

    ~IntDictImpl();

private:
    std::vector<std::vector<int>> seqs_;
    std::vector<int> parents_;

    int linearParse(const std::vector<int>& seq, std::vector<int>& builder,
                    const std::unordered_set<int>& excludes = nullptr);
    double weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double totalFreq,
                         std::vector<int>& builder, const std::unordered_set<int>& excludes = nullptr);
};


#endif //DICT_EXPANSION_INTDICTIMPL_H
