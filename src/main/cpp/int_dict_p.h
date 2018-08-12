//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_P_H
#define VGRAM_INT_DICT_P_H

#include "int_dict.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

class IntDictImpl : public IntDict {
public:
    IntDictImpl(const std::vector<std::vector<int>>& seqs);
    int search(const std::vector<int>& seq, const std::vector<int>* excludes = nullptr) const;
    std::vector<int> get(int index) const;
    int size() const;
    std::vector<std::vector<int>> alphabet() const;
    int parent(int second) const;

    int parse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq, std::vector<int>* output);
    int parse(const std::vector<int>& seq, const std::unordered_set<int>* excludes = nullptr, std::vector<int>* output);
    void weightParseVariants(const std::vector<int>& seq, double multiplier, const std::vector<int>& freqs,
                             double total_freq, const std::unordered_set<int>* excludes = nullptr,
                             std::unordered_map<int, double>* result);

    ~IntDictImpl();

private:
    std::vector<std::vector<int>> seqs_;
    std::vector<int> parents_;

    int linearParse(const std::vector<int>& seq, const std::unordered_set<int>* excludes = nullptr, std::vector<int>* builder);
    double weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq,
                         const std::unordered_set<int>* excludes = nullptr, std::vector<int>& builder);
};


#endif //VGRAM_INT_DICT_P_H
