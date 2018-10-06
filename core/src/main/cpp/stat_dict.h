//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYWITHSTAT_H
#define VGRAM_DICTIONARYWITHSTAT_H

#include <cmath>
#include <unordered_map>
#include "int_dict.h"
#include <memory>

class StatItem;

class StatDict
{
    friend class IntVGramBuilderImpl;
public:
    static constexpr double kMaxMinProbability = 0.002;
    const size_t kAggPower = 100000;

    explicit StatDict(double min_prob_result);
    StatDict(const std::vector<IntSeq>& seqs, double min_prob_result, IntSeq* init_freqs = nullptr);

    void update_symbol(int index, int freq);
    int search(const IntSeq& seq, std::unordered_set<int>* excludes = nullptr) const;
    const IntSeq& get(int index) const;
    int size() const;
    const std::vector<IntSeq>& alphabet() const;
    int parent(int second) const;
    int freq(int index) const;
    double code_length_per_char() const;

    bool enough(double probFound) const;
    int parse(const IntSeq& seq, IntSeq* output);
    void set_mutable(bool is_mutable);

private:
    std::shared_ptr<IntDict> dict_;
    IntSeq symbol_freqs_;
    IntSeq parse_freqs_;
    double power_ = 0;
    double parse_freqs_init_power_ = 0;
    std::unordered_map<std::int64_t, int> pairs_freqs_;
    double min_probability_;
    double total_chars_ = 0;
    bool is_mutable_ = true;

    double expand(int slots, std::vector<IntSeq>* new_dict, IntSeq* freqs) const;
    double reduce(int slots, std::vector<IntSeq>* new_dict, IntSeq* freqs) const;
    int filter_stat_items(int slots, std::vector<StatItem>* items) const;
    int stat_items(std::vector<StatItem>* items, std::unordered_set<int>* excludes) const;

//    int index_of_two_str(const IntSeq& first, const IntSeq& second, int betw, int ind);
//    bool is_substring(const IntSeq& s, const IntSeq& t);
//    void print_pairs(const std::unordered_map<std::int64_t, int>& old_pairs,
//                     const std::unordered_map<std::int64_t, int>& new_pairs) const ;

    //TODO: remove, make base class
    double weightedParse(const IntSeq& seq, const IntSeq& freqs, double total_freq,
                         IntSeq* result, std::unordered_set<int>* excludes = nullptr) const;

    void stat_item_to_text(const StatItem& item, IntSeq* output) const;

};

class StatItem {
public:
    StatItem(int first_, int second_, double score_, int count_);
    //std::string to_string();
    bool equals(const StatItem& statItem);
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

#endif //VGRAM_DICTIONARYWITHSTAT_H
