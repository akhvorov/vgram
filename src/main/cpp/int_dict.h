//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_H
#define VGRAM_INT_DICT_H

#import <vector>
#include <exception>
#include <unordered_set>

class DictionaryIndexIsCorruptedException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Dictionary index is corrupted!";
    }
};

typedef std::vector<int> IntSeq;

class IntDict {
public:
    IntDict() = default;
//    explicit IntDict(const std::vector<IntSeq>& seqs);
//    explicit IntDict(const IntSeq& seqs);

    virtual int search(const IntSeq& seq, std::unordered_set<int>* excludes = nullptr) const = 0;
    virtual const IntSeq& get(int index) const = 0;
    virtual int size() const = 0;
    virtual const std::vector<IntSeq>& alphabet() const = 0;
    virtual int parent(int second) const = 0;

    virtual int parse(const IntSeq& seq, const IntSeq& freqs, double total_freq, IntSeq* output) = 0;
    virtual int parse(const IntSeq& seq, IntSeq* output, std::unordered_set<int>* excludes = nullptr) = 0;
//    void weightParseVariants(const IntSeq& seq, double multiplier, const IntSeq& freqs,
//                             double total_freq, std::unordered_map<int, double>* result,
//                             std::unordered_set<int>* excludes);


  static IntDict* create(const std::vector<IntSeq>& seqs) {
      //return new IntDictImpl(seqs);
      return nullptr;
  }
};

#endif //VGRAM_LIST_DICT_H
