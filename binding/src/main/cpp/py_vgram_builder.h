//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include "src/main/cpp/int_vgram_builder.h"

#endif //DICT_EXPANSION_VGRAM_H

class PyVGramBuilder {
public:
    PyVGramBuilder(int size);
    PyVGramBuilder(const IntSeq& alphabet, int size);
    void accept(const IntSeq& seq);
    IntSeq parse(const IntSeq& seq);
    std::vector<IntSeq> alphabet() const;

private:
    std::shared_ptr<IntVGramBuilder> builder_;
};