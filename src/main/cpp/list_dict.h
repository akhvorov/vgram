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
  virtual void encode(const std::vector<T>& input, std::vector<int>* output) {
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
  int parse(const std::vector<T>& input, std::vector<int>* output, const std::vector<double>* freqs) {
    //TODO two ways: freqs = nullptr and other
    std::vector<T> temp(input.size());
    encoder.encode(input, &temp);
    double total_freq = std::accumulate(freqs.begin(), freqs.end(), 0.0);
    return delegate->parse(temp, output, freqs, total_freq);
  }

  static void save(const ListDict& dict) {

  }

private:
  IntDict* delegate = IntDict::create();
  Encoder<T> encoder;
};


#endif //VGRAM_LIST_DICT_H
