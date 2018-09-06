//
// Created by akhvorov on 24.07.18.
//

#include "int_dict_p.h"
#include <limits>
#include <numeric>
#include <iostream>

IntDictImpl::IntDictImpl(const std::vector<IntSeq>& seqs) {
    init(seqs);
}

IntDictImpl::IntDictImpl(const IntSeq& seqs) {
    std::vector<IntSeq> vec(seqs.size(), IntSeq(1));
    for (int i = 0; i < seqs.size(); i++) {
        vec[i][0] = seqs[i];
    }
    init(vec);
}

void IntDictImpl::init(const std::vector<IntSeq>& seqs) {
    seqs_ = std::vector<IntSeq>(seqs);
    parents_ = IntSeq(seqs_.size());
    std::vector<std::pair<IntSeq, int>> parents_stack;
    std::sort(seqs_.begin(), seqs_.end());

    for (int i = 0; i < seqs_.size(); ++i) {
        IntSeq current = seqs_[i];
        parents_[i] = -1;
        while (!parents_stack.empty()) {
            IntSeq prefix = parents_stack.back().first;
            if (std::mismatch(prefix.begin(), prefix.end(), current.begin()).first == prefix.end()) {
                parents_[i] = parents_stack.back().second;
                break;
            }
            parents_stack.pop_back();
        }
        parents_stack.emplace_back(std::pair<IntSeq, int>(current, i));
    }
}

IntDictImpl::IntDictImpl(const IntDictImpl& dict) {
    seqs_ = dict.seqs_;
    parents_ = dict.parents_;
}

int IntDictImpl::linearParse(const IntSeq& seq, IntSeq* builder, std::unordered_set<int>* excludes = nullptr) {
    IntSeq suffix = seq;
    while (!suffix.empty()) {
        int symbol;
        try {
            symbol = search(suffix, excludes);
            auto sym_len = get(symbol).size();
            suffix = IntSeq(suffix.begin() + sym_len, suffix.end());
        }
        catch (std::exception& e) {
            if (e.what() == DictionaryIndexIsCorruptedException().what()) {
                symbol = -1;
                suffix = IntSeq(suffix.begin() + 1, suffix.end());
            } else {
                throw e;
            }
        }
        builder->push_back(symbol);
    }
    return static_cast<int>(builder->size());
}

double IntDictImpl::weightedParse(const IntSeq& seq, const IntSeq& freqs, double total_freq,
                                  IntSeq* builder, std::unordered_set<int>* excludes = nullptr) {
    size_t len = seq.size();
    std::vector<double> score(len + 1, std::numeric_limits<double>::lowest());
    score[0] = 0;
    IntSeq symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        IntSeq suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = get(sym).size();
            double sym_log_prob = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(total_freq + size());

            if (score[sym_len + pos] < score[pos] + sym_log_prob) {
                score[sym_len + pos] = score[pos] + sym_log_prob;
                symbols[sym_len + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0);
    }
    IntSeq solution(len + 1);
    size_t pos = len;
    int index = 0;
    while (pos > 0) {
        int sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym).size();
    }
    for (int i = 0; i < index; i++) {
        builder->push_back(solution[len - index + i]);
    }
    return score[len];
}

void IntDictImpl::weightParseVariants(const IntSeq& seq, double multiplier, const IntSeq& freqs,
                                      double total_freq, std::unordered_map<int, double>* result,
                                      std::unordered_set<int>* excludes = nullptr) {
    auto len = seq.size();
    std::vector<double> count_forward(len + 1);
    {
        count_forward[0] = 1;
        for (int pos = 0; pos < len; pos++) {
            IntSeq suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                auto sym_len = get(sym).size();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                count_forward[pos + sym_len] += freq * count_forward[pos] / total_freq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    std::vector<double> count_backward(len + 1);
    {
        count_backward[len] = 1;
        for (auto pos = static_cast<int>(len - 1); pos >= 0; pos--) {
            IntSeq suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                auto sym_len = get(sym).size();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                count_backward[pos] += freq * count_backward[pos + sym_len] / total_freq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    for (int pos = 0; pos < len; pos++) {
        IntSeq suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = static_cast<int>(get(sym).size());
            int freq = sym < freqs.size() ? freqs[sym] : 0;
            double freq_increment = freq / total_freq * count_forward[pos] * count_backward[pos + sym_len];
            double v = multiplier * freq_increment / count_forward[len];
            if (result->count(sym) == 0) {
                (*result)[sym] = 0;
            }
            (*result)[sym] += v;
        }
        while ((sym = parent(sym)) >= 0);
    }
}

int IntDictImpl::search(const IntSeq& seq) const {
    return search(seq, nullptr);
}

int IntDictImpl::search(const IntSeq& seq, std::unordered_set<int>* excludes) const {
    int index = static_cast<int>(std::lower_bound(seqs_.begin(), seqs_.end(), seq) - seqs_.begin());
    if (seqs_[index] == seq) {
        if (excludes == nullptr || excludes->count(index) == 0)
            return index;
    }
    index--;
    while (index >= 0) {
        if (std::mismatch(seqs_[index].begin(), seqs_[index].end(), seq.begin()).first == seqs_[index].end() &&
            (excludes == nullptr || excludes->count(index) == 0))
            return index;
        index = parents_[index];
    }
    throw DictionaryIndexIsCorruptedException();
}

int IntDictImpl::parse(const IntSeq& seq, const IntSeq& freqs, double total_freq, IntSeq* output) {
    IntSeq result;
    double log_probability = weightedParse(seq, freqs, total_freq, output, nullptr);
    if (log_probability > 0) {
        for (int i : seq) {
            std::cout << i << " ";
        }
        std::cout << "->";
        for (int symbol : *output) {
            if (symbol >= 0) {
                // TODO redefine << operator for IntSeq
                std::cout << " ";
                for (int e : get(symbol))
                    std::cout << e;
            } else {
                std::cout << "##unknown##";
            }
        }
        std::cout << " " << log_probability << std::endl;
    }
    return static_cast<int>(output->size());
}

int IntDictImpl::parse(const IntSeq& seq, IntSeq* output, std::unordered_set<int>* excludes) {
    linearParse(seq, output, excludes);
    return static_cast<int>(output->size());
}

const IntSeq& IntDictImpl::get(int index) const {
    return seqs_[index];
}

int IntDictImpl::size() const {
    return static_cast<int>(seqs_.size());
}

const std::vector<IntSeq>& IntDictImpl::alphabet() const {
    return seqs_;
}

int IntDictImpl::parent(int second) const {
    return parents_[second];
}

IntDictImpl::~IntDictImpl() = default;