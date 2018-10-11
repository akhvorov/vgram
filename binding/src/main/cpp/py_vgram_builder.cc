//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cpp/int_vgram_builder_p.h>
#include <cpp/int_dict_p.h>
#include <numeric>
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size));
    dict_ = nullptr;
    freqs_ = IntSeq();
    total_freqs_ = 0;
    iter_num_ = iter_num;
}

PyVGramBuilder::PyVGramBuilder(std::string filename) {
    std::vector<IntSeq> seqs;
    std::ifstream file;
    file.open(filename);
    std::string line;
    while (std::getline(file, line)) {
        int freq = -1;
        IntSeq seq;
        size_t i = 0;
        for ( ; i < line.length() && line[i] != ')'; i++) {
            std::string tmp;
            while (i < line.length() && line[i] != ' ' && line[i] != '\t' && line[i] != ')') {
                tmp += line[i++];
            }
            int num;
            std::istringstream convert(tmp);
            if (!(convert >> num)) {
                num = 0;
                return;
            }
            if (line[i] == ')' || (line[i] == '\t' && freq != -1)) {
                seq.push_back(num);
                break;
            }
            if (freq == -1) {
                freq = num;
                i++;
            } else {
                seq.push_back(num);
            }
        }
        freqs_.push_back(freq);
        total_freqs_ += freq;
        seqs.push_back(seq);
    }
    file.close();
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
}

void PyVGramBuilder::save(const std::string& filename) const {
    std::ofstream file;
    file.open(filename);
    for (int i = 0; i < dict_->size(); i++) {
        file << freqs_[i] << "\t" << dict_->get(i) << std::endl;
    }
    file.close();
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
    for (int i = 0; i < iter_num_; i++) {
        std::next_permutation(mut_seqs.begin(), mut_seqs.end());
        for (const IntSeq& seq : mut_seqs) {
            builder_->accept(seq);
        }
    }
    dict_ = builder_->result();
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    for (int i = 0; i < iter_num_; i++) {
        for (const IntSeq& seq : seqs) {
            IntSeq result;
            dict_->parse(seq, freqs, total_freqs, &result);
            for (int symb : result) {
                total_freqs_++;
                freqs_[symb]++;
            }
        }
    }
}

std::vector<IntSeq> PyVGramBuilder::transform(const std::vector<IntSeq>& seqs) {
    std::vector<IntSeq> res;
    res.reserve(seqs.size());
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        dict_->parse(seq, freqs_, total_freqs_, &result);
        res.push_back(result);
    }
    return res;
}

std::vector<IntSeq> PyVGramBuilder::alphabet() const {
    return dict_->alphabet();
}