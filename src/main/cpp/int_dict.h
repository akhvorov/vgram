//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_H
#define VGRAM_INT_DICT_H

#import <vector>
#include <exception>

class IntDict {
public:
  const dictionary_index_is_corrupted_exception kDictionaryIndexIsCorrupted;

  virtual int parse(const std::vector<int>& input, const std::vector<int>& freqs, double total_freq, std::vector<int>* output) = 0;

  virtual int search(const std::vector<int>& seq, const std::unordered_set<int>* excludes = nullptr) const = 0;
  virtual std::vector<int>* get(int index) const = 0;
  virtual int size() const = 0;
  virtual std::vector<std::vector<int>>* alphabet() const = 0;
  virtual int parent(int second) const = 0;

  static IntDict* create();
};

class dictionary_index_is_corrupted_exception: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Dictionary index is corrupted!";
    }
};

#endif //VGRAM_LIST_DICT_H
