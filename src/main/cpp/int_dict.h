//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_DICT_P_H
#define DICT_EXPANSION_DICT_P_H

#import <vector>

class IntDict {
public:
  virtual int parse(const std::vector<int>& input, std::vector<int>* output, const std::vector<double>* freqs) = 0;

  static IntDict* create();
};

#endif //DICT_EXPANSION_DICT_P_H
