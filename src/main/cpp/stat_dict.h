//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYWITHSTAT_H
#define VGRAM_DICTIONARYWITHSTAT_H

#include <cmath>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "int_dict.h"
//#include "int_vgram_builder_p.h"

struct StatItem {
public:
    StatItem(std::int32_t first_, std::int32_t second_, double score_, std::int32_t count_);
    std::string to_string();
    bool equals(const StatItem& statItem);
    std::vector<int>* text();
    int first() const {
        return first_;
    }
    int second() const {
        return second_;
    }
    int score() const {
        return score_;
    }
    int count() const {
        return count_;
    }

private:
    int first_;
    int second_;
    double score_;
    int count_;
};

class StatDict
{
public:
    const double kMaxMinProbability = 0.002;
    const int kAggPower = 100000;

    StatDict(const IntDict& dictionary, double min_prob_result, std::vector<int>* init_freqs);

    void update_symbol(int index, int freq);
    int search(const std::vector<int>& seq) const ;
    int search(const std::vector<int>& seq, std::unordered_set<int>* excludes) const ;
    std::vector<int>* get(int index) const ;
    int size() const ;
    std::vector<std::vector<int>>* alphabet() const ;
    int parent(int second) const ;
    int freq(int index) const ;
    double code_length_per_char() const ;

    bool enough(double probFound);
    //void visitAssociations(std::int32_t start, TIntDoubleProcedure procedure); //TODO do it
    int parse(const std::vector<int>& seq, std::vector<int>* output);

private:
    std::shared_ptr<IntDict> dict_;
    //IntDict* dict_;
    std::vector<int> symbol_freqs_;
    std::vector<int> parse_freqs_;
    double power_ = 0;
    std::unordered_map<std::int64_t, int> pairs_freqs_;
    double min_probability_;
    double total_chars_ = 0;
    //FastRandom rng = new FastRandom(0);

    StatDict* reduce(std::int32_t slots);
    std::vector<StatItem> filter_stat_items(int slots);
    std::vector<StatItem> stat_items(std::unordered_set<int>* excludes);

    int index_of_two_str(const std::vector<int>& first, const std::vector<int>& second, int betw, int ind);
    bool is_substring(const std::vector<int>& s, const std::vector<int>& t);

    void print_pairs(const std::unordered_map<std::int64_t, int>& old_pairs,
                     const std::unordered_map<std::int64_t, int>& new_pairs) const ;

    StatDict* expand(int slots);

};

#endif //VGRAM_DICTIONARYWITHSTAT_H
