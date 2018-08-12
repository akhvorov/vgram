//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_VGRAM_BUILDER_H
#define VGRAM_VGRAM_BUILDER_H


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
  IntVGramBuilder* delegate_;
  ListDict<T> current_;
};


#endif //VGRAM_VGRAM_BUILDER_H
