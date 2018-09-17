//
// Created by akhvorov on 18.09.18.
//

#include "list_dict.h"
#include "int_dict_p.h"

template <class T>
ListDict<T>::ListDict(int alphabet_size) {
  std::vector<int> seqs(static_cast<unsigned long>(alphabet_size));
  for (int i = 0; i < alphabet_size; i++) {
      seqs[i] = i;
  }
  delegate_ = new IntDictImpl(seqs);
}