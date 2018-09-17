//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_LIST_DICT_H
#define VGRAM_LIST DICT_H

#include <vector>
#include <unordered_map>
#include <numeric>

#include "int_dict.h"

template <typename T>
class Encoder {
public:
  virtual void encode(const std::vector<T>& input, IntSeq* output) {
    for (const T& next: input) {
      auto code_ptr = mapping.find(next);
      if (code_ptr == mapping.end()) {
        mapping[next] = next_id++;
      }
      else output->push_back(*code_ptr);
    }
  }
  
private:
  std::unordered_map<T, int> mapping;
  int next_id = 0;
};

template <typename T>
class ListDict {
public:
  ListDict(std::vector<std::vector<T>> seqs) {
      delegate_ = new IntDictImpl(seqs);
  }
  ListDict(std::vector<T> seqs) {
      delegate_ = new IntDictImpl(seqs);
  }

  int parse(const std::vector<T>& input, const std::vector<double>& freq, IntSeq* output) {
    //TODO two ways: freqs = nullptr and other
    std::vector<T> temp(input.size());
    encoder_.encode(input, &temp);
    double total_freq = std::accumulate(freqs.begin(), freqs.end(), 0.0);
    return delegate_->parse(temp, freqs, total_freq, output);
  }

  static void save(const ListDict& dict) {

  }

private:
  IntDict* delegate_;
  Encoder<T> encoder_;
};


#endif //VGRAM_LIST_DICT_H
