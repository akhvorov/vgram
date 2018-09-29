//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#include <src/main/cpp/int_vgram_builder_p.h>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size));
}

PyVGramBuilder::PyVGramBuilder(const IntSeq& alphabet, int size) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(alphabet, size));
}

void PyVGramBuilder::accept(const IntSeq& seq) {
    builder_->accept(seq);
}

IntSeq PyVGramBuilder::parse(const IntSeq& seq) {
    IntSeq result;
    builder_->result()->parse(seq, &result);
    return result;
}

std::vector<IntSeq> PyVGramBuilder::alphabet() const {
    return builder_->result()->alphabet();
}