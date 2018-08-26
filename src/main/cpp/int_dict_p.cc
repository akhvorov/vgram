//
// Created by akhvorov on 24.07.18.
//

#include "int_dict_p.h"
#include <limits>
#include <numeric>
#include <iostream>

IntDictImpl::IntDictImpl(const std::vector<std::vector<int>>& seqs) {
    seqs_ = std::vector<std::vector<int>>(seqs);
    parents_ = std::vector<int>(seqs.size());
    std::vector<std::pair<std::vector<int>, int>> parents_stack;
    std::sort(seqs.begin(), seqs.end());

    for (int i = 0; i < seqs.size(); ++i) {
        std::vector<int> current = seqs[i];
        parents_[i] = -1;
        while (!parents_.empty()) {
            std::vector<int> prefix = parents_stack.back().first;
            if (std::mismatch(prefix.begin(), prefix.end(), current.begin()).first == prefix.end()) {
                parents_[i] = parents_stack.back().second;
                break;
            }
            parents_stack.pop_back();
        }
        parents_stack.push_back((current, i));
    }
}

int IntDictImpl::linearParse(const std::vector<int>& seq, std::vector<int>* builder, const std::unordered_set<int>* excludes = nullptr) {
    std::vector<int> suffix = seq;
    while (!suffix.empty()) {
        int symbol;
        try {
            symbol = search(suffix, excludes);
            auto sym_len = get(symbol)->size();
            suffix = std::vector<int>(suffix.begin() + sym_len, suffix.end());
        }
        catch (std::exception& e) {
            if (e.what() == IntDict::kDictionaryIndexIsCorrupted.what()) {
                symbol = -1;
                suffix = std::vector<int>(suffix.begin() + 1, suffix.end());
            } else {
                throw e;
            }
        }
        builder->push_back(symbol);
    }
    return static_cast<int>(builder->size());
}

double IntDictImpl::weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq,
                                  std::vector<int>* builder, const std::unordered_set<int>* excludes = nullptr) {
    int len = seq.size();
    std::vector<double> score(len + 1, std::numeric_limits<double>::min());
    score[0] = 0;
    std::vector<int> symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        std::vector<int> suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = get(sym)->size();
            double symLogProb = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(total_freq + size());

            if (score[sym_len + pos] < score[pos] + symLogProb)
            {
                score[sym_len + pos] = score[pos] + symLogProb;
                symbols[sym_len + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0);
    }
    std::vector<int> solution(len + 1);
    int pos = len;
    int index = 0;
    while (pos > 0) {
        int sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym)->size();
    }
    for (int i = 0; i < index; i++) {
        builder->push_back(solution[len - index + i]);
    }
    return score[len];
}

void IntDictImpl::weightParseVariants(const std::vector<int>& seq, double multiplier, const std::vector<int>& freqs,
                                      double total_freq, std::unordered_map<int, double>* result,
                                      const std::unordered_set<int>* excludes = nullptr) {
    int len = seq.size();
    std::vector<double> count_forward(len + 1);
    {
        count_forward[0] = 1;
        for (int pos = 0; pos < len; pos++) {
            std::vector<int> suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                auto sym_len = get(sym)->size();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                count_forward[pos + sym_len] += freq * count_forward[pos] / total_freq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    std::vector<double> count_backward(len + 1);
    {
        count_backward[len] = 1;
        for (int pos = len - 1; pos >= 0; pos--) {
            std::vector<int> suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                auto sym_len = get(sym)->size();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                count_backward[pos] += freq * count_backward[pos + sym_len] / total_freq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    for (int pos = 0; pos < len; pos++) {
        std::vector<int> suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = get(sym)->size();
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

int IntDictImpl::search(const std::vector<int>& seq, const std::unordered_set<int>* excludes = nullptr) const {
    unsigned long index = std::lower_bound(seqs_.begin(), seqs_.end(), seq) - seqs_.begin(); // TODO maybe it work in another way
    if (index >= 0) {
        if (excludes == nullptr || excludes->count(index) == 0)
            return index;
        index = -(parents_[index] + 2);
    }
    index = -(index + 2);
    while (index >= 0) {
        if (std::mismatch(seqs_[index].begin(), seqs_[index].end(), seq.begin()).first == seqs_[index].end() &&
            (excludes == nullptr || excludes->count(index) == 0))
            return index;
        index = parents_[index];
    }
    throw std::exception(IntDict::kDictionaryIndexIsCorrupted);
}

int IntDictImpl::parse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq, std::vector<int>* output) {
    std::vector<int> result;
    double logProBab = weightedParse(seq, freqs, total_freq, output, nullptr);
    if (logProBab > 0) {
        for (int i = 0; i < seq.size(); i++) {
            std::cout << seq[i] << " ";
        }
        std::cout << "->";
        for (int i = 0; i < output->size(); i++) {
            int symbol = (*output)[i];
            if (symbol >= 0)
                std::cout << " " << get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " << logProBab << std::endl;
    }
    return output->size();
}

int IntDictImpl::parse(const std::vector<int>& seq, std::vector<int>* output, const std::unordered_set<int>* excludes = nullptr) {
    linearParse(seq, output, excludes);
    return output->size();
}

std::vector<int>* IntDictImpl::get(int index) const {
    return &seqs_[index];
}

int IntDictImpl::size() const {
    return seqs_.size();
}

std::vector<std::vector<int>>* IntDictImpl::alphabet() const {
    return &seqs_;
}

int IntDictImpl::parent(int second) const {
    return parents_[second];
}

IntDictImpl::~IntDictImpl() {}