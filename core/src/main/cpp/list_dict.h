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
      output->push_back(code_ptr->second);
    }
  }

  int size() const {
      return next_id;
  }
  
private:
  std::unordered_map<T, int> mapping;
  int next_id = 0;
};

template <typename T>
class ListDict {
public:
  ListDict(int alphabet_size);

  int parse(const std::vector<T>& input, const std::vector<double>& freqs, IntSeq* output) {
    std::vector<T> temp(input.size());
    encoder_.encode(input, &temp);
    double total_freq = std::accumulate(freqs.begin(), freqs.end(), 0.0);
    return delegate_->parse(temp, freqs, total_freq, output);
  }

private:
  IntDict* delegate_;
  Encoder<T> encoder_;
};


#endif //VGRAM_LIST_DICT_H
