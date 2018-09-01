//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_P_H
#define VGRAM_INT_DICT_P_H

#include "int_dict.h"
#include <unordered_map>

class IntDictImpl : public IntDict {
public:
    explicit IntDictImpl(const std::vector<std::vector<int>>& seqs);
    IntDictImpl(const IntDictImpl& dict);

    int search(const std::vector<int>& seq) const override;
    int search(const std::vector<int>& seq, std::unordered_set<int>* excludes) const override;
    const std::vector<int>* get(int index) const override;
    int size() const override;
    const std::vector<std::vector<int>>* alphabet() const override;
    int parent(int second) const override;

    int parse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq, std::vector<int>* output) override;
    int parse(const std::vector<int>& seq, std::vector<int>* output, std::unordered_set<int>* excludes = nullptr);
    void weightParseVariants(const std::vector<int>& seq, double multiplier, const std::vector<int>& freqs,
                             double total_freq, std::unordered_map<int, double>* result,
                             std::unordered_set<int>* excludes);

    ~IntDictImpl();

private:
    std::vector<std::vector<int>> seqs_;
    std::vector<int> parents_;

    int linearParse(const std::vector<int>& seq, std::vector<int>* builder, std::unordered_set<int>* excludes);
    double weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq,
            std::vector<int>* builder, std::unordered_set<int>* excludes);
};


#endif //VGRAM_INT_DICT_P_H
