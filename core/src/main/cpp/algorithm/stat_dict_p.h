//
// Created by akhvorov on 01.07.18.
//

#ifndef DICT_EXPANSION_STAT_DICT_P_H
#define DICT_EXPANSION_STAT_DICT_P_H

#include <cmath>
#include <unordered_map>
#include <memory>
#include "int_dict.h"
#include "stat_dict.h"

class StatItem;

class StatDictImpl : StatDict {
    friend class IntVGramBuilderImpl;

public:
    static constexpr double kMaxMinProbability = 0.002;
    const size_t kAggPower = 100000;

    explicit StatDictImpl(double min_probability = kMaxMinProbability);

    explicit StatDictImpl(const std::vector<IntSeq> &seqs, double min_prob_result = kMaxMinProbability,
                          IntSeq *init_freqs = nullptr);

    void update_symbol(int index, int freq) override;

    int search(const IntSeq &seq, std::unordered_set<int> *excludes) const override;

    int search(const IntSeq &seq) const override;

    const IntSeq &get(int index) const override;

    int size() const override;

    const std::vector<IntSeq> &alphabet() const override;

    int parent(int second) const override;

    int freq(int index) const override;

    double code_length_per_char() const override;

    bool enough(double probFound) const override;

    int parse(const IntSeq &seq, IntSeq *output) override;

private:
    std::shared_ptr<IntDict> dict_;
    IntSeq symbol_freqs_;
    IntSeq parse_freqs_;
    double power_ = 0;
    double parse_freqs_init_power_ = 0;
    std::unordered_map<std::int64_t, int> pairs_freqs_;
    double min_probability_;
    double total_chars_ = 0;

    double expand(int slots, std::vector<IntSeq> *new_dict, IntSeq *freqs) const;

    double reduce(int slots, std::vector<IntSeq> *new_dict, IntSeq *freqs) const;

    int filter_stat_items(int slots, std::vector<StatItem> *items) const;

    int stat_items(std::vector<StatItem> *items, std::unordered_set<int> *excludes) const;

    //TODO: it's copy of int_dict method
    double weightedParse(const IntSeq &seq, const IntSeq &freqs, double total_freq,
                         IntSeq *result, std::unordered_set<int> *excludes = nullptr) const;

    void stat_item_to_text(const StatItem &item, IntSeq *output) const;
};

class StatItem {
public:
    StatItem(int first_, int second_, double score_, int count_);

    bool equals(const StatItem &statItem);

    int first() const;

    int second() const;

    double score() const;

    int count() const;

private:
    int first_;
    int second_;
    double score_;
    int count_;
};

#endif //DICT_EXPANSION_STAT_DICT_P_H
