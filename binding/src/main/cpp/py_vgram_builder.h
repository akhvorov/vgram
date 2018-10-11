//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <cpp/int_vgram_builder.h>

class PyVGramBuilder {
public:
//    PyVGramBuilder(int size);
//    PyVGramBuilder(const IntSeq& alphabet, int size);
//    void accept(const IntSeq& seq);
//    IntSeq parse(const IntSeq& seq);

    PyVGramBuilder(int size, int iter_num);
    explicit PyVGramBuilder(std::string filename);
    void save(const std::string& filename) const;
    //PyVGramBuilder(int size, IntSeq* alphabet = nullptr, int iter_num = 1);
    //PyVGramBuilder(int size, IntSeq* alphabet, int iter_num);
    void fit(const std::vector<IntSeq>& seqs);
    std::vector<IntSeq> transform(const std::vector<IntSeq>& seqs);

    std::vector<IntSeq> alphabet() const;

private:
    int iter_num_;
    std::shared_ptr<IntVGramBuilder> builder_;
    std::shared_ptr<IntDict> dict_;
    IntSeq freqs_;
    int total_freqs_;
};

#endif //DICT_EXPANSION_VGRAM_H