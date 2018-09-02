//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYWITHSTAT_H
#define VGRAM_DICTIONARYWITHSTAT_H

#include <cmath>
#include <unordered_map>
#include "int_dict_p.h"
#include "int_vgram_builder_p.h" //!!!


class StatItem;

class StatDict
{
public:
    const double kMaxMinProbability = 0.002;
    const int kAggPower = 100000;

    StatDict(const std::vector<std::vector<int>>& seqs, double min_prob_result, std::vector<int>* init_freqs = nullptr);

    void update_symbol(int index, int freq);
    int search(const std::vector<int>& seq, std::unordered_set<int>* excludes = nullptr) const;
    const std::vector<int>* get(int index) const;
    int size() const;
    const std::vector<std::vector<int>>* alphabet() const;
    int parent(int second) const;
    int freq(int index) const;
    double code_length_per_char() const;

    bool enough(double probFound);
    //void visitAssociations(std::int32_t start, TIntDoubleProcedure procedure); //TODO do it
    int parse(const std::vector<int>& seq, std::vector<int>* output);

private:
    std::shared_ptr<IntDictImpl> dict_;
    //IntDict* dict_;
    std::vector<int> symbol_freqs_;
    std::vector<int> parse_freqs_;
    double power_ = 0;
    std::unordered_map<std::int64_t, int> pairs_freqs_;
    double min_probability_;
    double total_chars_ = 0;
    //FastRandom rng = new FastRandom(0);

    StatDict reduce(int slots);
    std::vector<StatItem> filter_stat_items(int slots);
    std::vector<StatItem> stat_items(std::unordered_set<int>* excludes);

    int index_of_two_str(const std::vector<int>& first, const std::vector<int>& second, int betw, int ind);
    bool is_substring(const std::vector<int>& s, const std::vector<int>& t);

    //TODO: remove, make base class
    double weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq,
                         std::vector<int>* builder, std::unordered_set<int>* excludes = nullptr);

    void print_pairs(const std::unordered_map<std::int64_t, int>& old_pairs,
                     const std::unordered_map<std::int64_t, int>& new_pairs) const ;

    StatDict expand(int slots);

};

class StatItem {
public:
    StatItem(int first_, int second_, double score_, int count_);
    //std::string to_string();
    bool equals(const StatItem& statItem);
    void text(std::vector<int>* output) const;
    int first() const;
    int second() const;
    double score() const;
    int count() const;

private:
    StatDict* stat_dict_;
    int first_;
    int second_;
    double score_;
    int count_;
};

#endif //VGRAM_DICTIONARYWITHSTAT_H
