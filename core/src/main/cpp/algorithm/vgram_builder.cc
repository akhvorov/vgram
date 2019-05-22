//
// Created by Aleksandr Khvorov on 19/09/2018.
//

#include "vgram_builder.h"
#include "int_vgram_builder_p.h"

template <typename T>
VGramBuilder<T>::VGramBuilder(int size) {
    delegate_ = new IntVGramBuilderImpl(size);
    current_ = new ListDict<T>;
}
