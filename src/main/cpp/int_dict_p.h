//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_P_H
#define VGRAM_INT_DICT_P_H

#include "int_dict.h"
#include <unordered_map>

class IntDictImpl : public IntDict {
public:
    explicit IntDictImpl(const std::vector<IntSeq>& seqs);
    explicit IntDictImpl(const IntSeq& seqs);
    IntDictImpl(const IntDictImpl& dict);

    int search(const IntSeq& seq) const override;
    int search(const IntSeq& seq, std::unordered_set<int>* excludes) const override;
    const IntSeq& get(int index) const override;
    int size() const override;
    const std::vector<IntSeq>& alphabet() const override;
    int parent(int second) const override;

    int parse(const IntSeq& seq, const IntSeq& freqs, double total_freq, IntSeq* output) override;
    int parse(const IntSeq& seq, IntSeq* output, std::unordered_set<int>* excludes = nullptr);
    void weightParseVariants(const IntSeq& seq, double multiplier, const IntSeq& freqs,
                             double total_freq, std::unordered_map<int, double>* result,
                             std::unordered_set<int>* excludes);

    ~IntDictImpl();

private:
    std::vector<IntSeq> seqs_;
    IntSeq parents_;

    void init(const std::vector<IntSeq>& seqs);
    int linearParse(const IntSeq& seq, IntSeq* builder, std::unordered_set<int>* excludes);
    double weightedParse(const IntSeq& seq, const IntSeq& freqs, double total_freq,
                         IntSeq* builder, std::unordered_set<int>* excludes);
};


#endif //VGRAM_INT_DICT_P_H
