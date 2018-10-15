//
// Created by Aleksandr Khvorov on 15/10/2018.
//

#ifndef DICT_EXPANSION_STAT_DICT_H
#define DICT_EXPANSION_STAT_DICT_H

#include "int_dict.h"

class StatDict {
public:
    virtual void update_symbol(int index, int freq) = 0;
    virtual int search(const IntSeq& seq, std::unordered_set<int>* excludes) const = 0;
    virtual int search(const IntSeq& seq) const = 0;
    virtual const IntSeq& get(int index) const = 0;
    virtual int size() const = 0;
    virtual const std::vector<IntSeq>& alphabet() const = 0;
    virtual int parent(int second) const = 0;
    virtual int freq(int index) const = 0;
    virtual double code_length_per_char() const = 0;
    virtual bool enough(double probFound) const = 0;
    virtual int parse(const IntSeq& seq, IntSeq* output) = 0;
};

#endif //DICT_EXPANSION_STAT_DICT_H
