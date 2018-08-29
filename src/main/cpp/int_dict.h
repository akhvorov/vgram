//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_DICT_H
#define VGRAM_INT_DICT_H

#import <vector>
#include <exception>
#include <unordered_set>

class dictionary_index_is_corrupted_exception: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Dictionary index is corrupted!";
    }
};

class IntDict {
public:
  virtual int parse(const std::vector<int>& input, const std::vector<int>& freqs, double total_freq, std::vector<int>* output) = 0;

  virtual int search(const std::vector<int>& seq, std::unordered_set<int>* excludes) const = 0;
  virtual std::vector<int>* get(int index) const = 0;
  virtual int size() const = 0;
  virtual std::vector<std::vector<int>>* alphabet() const = 0;
  virtual int parent(int second) const = 0;

  static IntDict* create();
};

#endif //VGRAM_LIST_DICT_H
