//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include "int_vgram_builder_p.h"
#include "int_dict_p.h"
#include "py_vgram_builder.h"

PyVGramBuilder::PyVGramBuilder(int size, int iter_num) {
    builder_ = std::shared_ptr<IntVGramBuilder>(new IntVGramBuilderImpl(size - 1));
    dict_ = nullptr;
    coder_ = SeqCoder();
    freqs_ = IntSeq();
    total_freqs_ = 0;
    iter_num_ = iter_num;
    fitted_ = false;
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
                i++;
            } else {
                seq.push_back(num);
            }
        }
        freqs_.push_back(freq);
        total_freqs_ += freq;
        seqs.push_back(coder_.encode(seq));
    }
    std::cout << "Real size: "<< freqs_.size() << std::endl;
    file.close();
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl(seqs));
    iter_num_ = 0;
    fitted_ = true;
}

void PyVGramBuilder::save(const std::string& filename, BaseTokenizer* tokenizer) const {
    std::ofstream file;
    file.open(filename);
    for (int i = 0; i < dict_->size(); i++) {
        file << freqs_[i] << "\t" << coder_.decode(dict_->get(i)) << "\t" << tokenizer->decode(std::vector<std::vector<int>>(1, coder_.decode(dict_->get(i))))[0] << std::endl;
    }
    file.close();
}

IntSeq PyVGramBuilder::freqs() const {
    return freqs_;
}

PyVGramBuilder* PyVGramBuilder::fit(const std::vector<IntSeq>& seqs, py::args args) {
    if (fitted_) {
        return this;
    }
    for (int i = 0; i < iter_num_; i++) {
        std::cout << i << "-th iteration" << std::endl;
        for (int k = 0; k < seqs.size(); k++) {
            IntSeq s = coder_.encode(seqs[rand() % static_cast<int>(seqs.size())]);
            builder_->accept(s);
        }
    }
    compute_freqs(seqs);
    fitted_ = true;
    return this;
}

void PyVGramBuilder::compute_freqs(const std::vector<IntSeq>& seqs) {
    std::cout << "compute freqs" << std::endl;
    dict_ = builder_->result();
    IntSeq freqs;
    builder_->result_freqs(&freqs);
    freqs_ = IntSeq(freqs.size(), 0);
    int total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        dict_->parse(coder_.encode(seq), freqs, total_freqs, &result);
        for (int symb : result) {
            total_freqs_++;
            freqs_[symb]++;
        }
    }
}

std::vector<std::string> PyVGramBuilder::transform(const std::vector<IntSeq>& seqs, py::args args) const {
    std::vector<std::string> res;
    res.reserve(seqs.size());
    for (const IntSeq& seq : seqs) {
        IntSeq result;
        std::string str;
        dict_->parse(coder_.encode(seq), freqs_, total_freqs_, &result);
        for (const auto& i : result) {
            str += " " + std::to_string(i);
        }
        if (str.length() == 0) {
            str = "-1";
        }
        res.push_back(str);
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
