//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_H
#define VGRAM_INT_DICT_H

#import <vector>
#include <exception>
#include <unordered_set>

typedef std::vector<int> IntSeq;

std::ostream& operator<< (std::ostream &out, const IntSeq& seq);
std::string to_string(const IntSeq& seq);

class IntDict {
public:
    IntDict() = default;
    virtual ~IntDict() = default;

    virtual int search(const IntSeq& seq, std::unordered_set<int>* excludes = nullptr) = 0;
    virtual const IntSeq& get(int index) const = 0;
    virtual int size() const = 0;
    virtual const std::vector<IntSeq>& alphabet() const = 0;
    virtual int parent(int second) const = 0;

    virtual int parse(const IntSeq& seq, const IntSeq& freqs, double total_freq, IntSeq* output) = 0;
    virtual int parse(const IntSeq& seq, IntSeq* output, std::unordered_set<int>* excludes = nullptr) = 0;
};

#endif //VGRAM_LIST_DICT_H
