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
    coder_ = SeqCoder();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    iter_num_ = iter_num;
}

PyVGramBuilder::PyVGramBuilder(std::string filename) {
    coder_ = SeqCoder();
    std::vector<IntSeq> seqs;
    std::ifstream file;
    file.open(filename);
    std::string line;
    while (std::getline(file, line)) {
        int freq = -1;
        IntSeq seq;
        size_t i = 0;
        bool not_include = false;
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
                if (num == 0) {
                    not_include = true;
                    break;
                }
                i++;
            } else {
                seq.push_back(num);
            }
        }
        if (!not_include) {
            freqs_.push_back(freq);
            total_freqs_ += freq;
            seqs.push_back(coder_.encode(seq));
        }
    }
    std::cout << "Real size: "<< freqs_.size() << std::endl;
    file.close();
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
    iter_num_ = 0;
}

void PyVGramBuilder::save(const std::string& filename) const {
    std::ofstream file;
    file.open(filename);
    for (int i = 0; i < dict_->size(); i++) {
        file << freqs_[i] << "\t" << coder_.decode(dict_->get(i)) << std::endl;
    }
    file.close();
}

IntSeq PyVGramBuilder::freqs() const {
    return freqs_;
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
    for (int i = 0; i < iter_num_; i++) {
        std::cout << i << "-th iteration" << std::endl;
        for (int k = 0; k < seqs.size(); k++) {
            IntSeq s = coder_.encode(seqs[rand() % static_cast<int>(seqs.size())]);
            builder_->accept(s);
        }
    }
    compute_freqs(seqs);
}

void PyVGramBuilder::compute_freqs(const std::vector<IntSeq>& seqs) {
    dict_ = builder_->result();
//    builder_->result_freqs(&freqs_);
//    total_freqs_ = std::accumulate(freqs_.begin(), freqs_.end(), 0);
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    std::cout << "last iteration" << std::endl;
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        dict_->parse(coder_.encode(seq), freqs, total_freqs, &result);
        for (int symb : result) {
            total_freqs_++;
            freqs_[symb]++;
        }
    }
}

std::vector<IntSeq> PyVGramBuilder::transform(const std::vector<IntSeq>& seqs) {
    std::vector<IntSeq> res;
    res.reserve(seqs.size());
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        dict_->parse(coder_.encode(seq), freqs_, total_freqs_, &result);
        res.push_back(result);
    }
    return res;
}

std::vector<IntSeq> PyVGramBuilder::alphabet() const {
    std::vector<IntSeq> alpha;
    for (const auto &a : dict_->alphabet()) {
        alpha.push_back(coder_.decode(a));
    }
    return alpha;
}