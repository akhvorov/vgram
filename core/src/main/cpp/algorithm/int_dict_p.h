//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_P_H
#define VGRAM_INT_DICT_P_H

#include <unordered_map>
#include "int_dict.h"

class IntDictImpl : public IntDict {
public:
    IntDictImpl();
    explicit IntDictImpl(const std::vector<IntSeq>& seqs);
    explicit IntDictImpl(const IntSeq& seqs);
    IntDictImpl(const IntDictImpl& dict);

    int search(const IntSeq& seq, std::unordered_set<int>* excludes) override;
    const IntSeq& get(int index) const override;
    int size() const override;
    const std::vector<IntSeq>& alphabet() const override;
    int parent(int second) const override;

    int parse(const IntSeq& seq, const IntSeq& freqs, double total_freq, IntSeq* output) override;
    int parse(const IntSeq& seq, IntSeq* output, std::unordered_set<int>* excludes) override;

    ~IntDictImpl() override;

private:
    std::vector<IntSeq> seqs_;
    IntSeq parents_;

    void init(const std::vector<IntSeq>& seqs);
    int linearParse(const IntSeq& seq, IntSeq* builder, std::unordered_set<int>* excludes);
    double weightedParse(const IntSeq& seq, const IntSeq& freqs, double total_freq,
                         IntSeq* builder, std::unordered_set<int>* excludes);
};

#endif //VGRAM_INT_DICT_P_H
