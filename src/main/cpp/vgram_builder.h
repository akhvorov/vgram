//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_VBUILDER_H
#define DICT_EXPANSION_VBUILDER_H


#include <vector>
#include "list_dict.h"

class IntVGramBuilder;

template <typename T>
class VGramBuilder {
public:
  void accept(const std::vector<T>& input) {
    
  }
  
  ListDict<T>* build(std::vector<double>* freqs = nullptr) {
    
  }

private:
  IntVGramBuilder* delegate;
  ListDict<T> current;
};


#endif //DICT_EXPANSION_VBUILDER_H
