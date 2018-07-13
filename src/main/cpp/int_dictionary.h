//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_DICT_P_H
#define DICT_EXPANSION_DICT_P_H

class IntDictionary {
public:
  virtual int parse(const std::vector<int>& input, std::vector<int>* output, const std::vector<double>* freqs) = 0;

  static IntDictionary* create();
};

#endif //DICT_EXPANSION_DICT_P_H
