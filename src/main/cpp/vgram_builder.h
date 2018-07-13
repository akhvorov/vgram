//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef DICT_EXPANSION_VBUILDER_H
#define DICT_EXPANSION_VBUILDER_H


#include <vector>
#include "list_dictionary.h"

class VGramIntBuilder;

template <typename T>
class VGramBuilder {
public:
  void accept(const std::vector<T>& input) {
    
  }
  
  ListDictionary<T>* build(std::vector<double>* freqs = nullptr) {
    
  }

private:
  VGramIntBuilder* delegate;
  ListDictionary<T> current;
};


#endif //DICT_EXPANSION_VBUILDER_H
