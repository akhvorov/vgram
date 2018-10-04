//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#include <algorithm>
#include <cpp/int_vgram_builder_p.h>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size));
    stat_dict_ = nullptr;
    iter_num_ = iter_num;
}
//
//PyVGramBuilder::PyVGramBuilder(const IntSeq& alphabet, int size) {
//    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(alphabet, size));
//}
//
//void PyVGramBuilder::accept(const IntSeq& seq) {
//    builder_->accept(seq);
//}
//
//IntSeq PyVGramBuilder::parse(const IntSeq& seq) {
//    IntSeq result;
//    builder_->result()->parse(seq, &result);
//    return result;
//}

//PyVGramBuilder::PyVGramBuilder(int size, IntSeq* alphabet, int iter_num) {
//    if (alphabet == nullptr) {
//        builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size));
//    } else {
//        builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(*alphabet, size));
//    }
//    iter_num_ = iter_num;
//    stat_dict_ = nullptr;
//}

void PyVGramBuilder::fit(const std::vector<IntSeq>& seqs) {
    std::vector<IntSeq> mut_seqs(seqs);
    if (stat_dict_ != nullptr)
        stat_dict_->set_mutable(true);
    for (int i = 0; i < iter_num_; i++) {
        std::next_permutation(mut_seqs.begin(), mut_seqs.end());
        for (const IntSeq& seq : mut_seqs) {
            builder_->accept(seq);
        }
    }
    stat_dict_ = builder_->result();
}

std::vector<IntSeq> PyVGramBuilder::transform(const std::vector<IntSeq>& seqs) {
    stat_dict_->set_mutable(false);
    std::vector<IntSeq> res;
    res.reserve(seqs.size());
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        stat_dict_->parse(seq, &result);
        res.push_back(result);
    }
    return res;
}

std::vector<IntSeq> PyVGramBuilder::alphabet() const {
    return stat_dict_->alphabet();
}