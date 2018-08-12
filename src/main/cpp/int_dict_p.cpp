//
// Created by akhvorov on 24.07.18.
//

#include "int_dict_p.h"
#include <limits>
#include <numeric>

IntDictImpl::IntDictImpl(const std::vector<std::vector<int>>& seqs) {
    seqs_ = std::vector<std::vector<int>>(seqs);
    parents_ = std::vector<int>(seqs.size());
    std::vector<std::pair<std::vector<int>, int>> parents_stack;
    std::sort(sex.begin(), sex.end());

    for (int i = 0; i < sex.size(); ++i) {
        std::vector<int> current = sex[i];
        parents_[i] = -1;
        while (!parents_.empty()) {
            std::vector<int> prefix = parents_stack.back().first;
            if (std::mismatch(prefix.begin(), prefix.end(), current.begin()).first == prefix.end()) {
                parents_[i] = parents_stack.back().second;
                break;
            }
            parents_stack.pop_back();
        }
        parents_stack.push_back(std::pair<std::vector<int>, int>(current, i));
    }
}

int IntDictImpl::linearParse(const std::vector<int>& seq, std::vector<int>& builder,
                std::unordered_set<int> const & excludes = nullptr) {
    std::vector<int> suffix = seq;
    while (suffix.size() > 0) {
        int symbol;
        try {
            symbol = search(suffix, excludes);
            int symLen = get(symbol).length();
            suffix = suffix.substr(symLen, suffix.length() - symLen);
        }
        catch (int e_code) { //TODO: exceptions
            if (DICTIONARY_INDEX_IS_CORRUPTED_CODE == e_code) {
                symbol = -1;
                suffix = suffix.substr(1, suffix.length() - 1);
            }
            else throw e_code;
        }
        builder.push_back(symbol);
    }
    return builder.size();
}

double IntDictImpl::weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double totalFreq,
                                  std::vector<int>& builder, conststd::unordered_set<int>& excludes = nullptr) {
    int len = seq.size();
    std::vector<double> score(len + 1, std::numeric_limits<double>::min());
    score[0] = 0;
    std::vector<int> symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        std::vector<int> suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            int symLen = get(sym).size();
            double symLogProb = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(totalFreq + size());

            if (score[symLen + pos] < score[pos] + symLogProb)
            {
                score[symLen + pos] = score[pos] + symLogProb;
                symbols[symLen + pos] = sym;
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
        pos -= get(sym).size();
    }
    for (int i = 0; i < index; i++) {
        builder.push_back(solution[len - index + i]);
    }
    return score[len];
}

void IntDictImpl::weightParseVariants(const std::vector<int>& seq, double multiplier, const std::vector<int>& freqs,
                                      double totalFreq, const std::unordered_set<int>& excludes,
                                      std::unordered_map<int, double>& result) {
    int len = seq.size();
    std::vector<double> countForward(len + 1);
    {
        countForward[0] = 1;
        for (int pos = 0; pos < len; pos++) {
            std::vector<int> suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                int symLen = get(sym).length();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                countForward[pos + symLen] += freq * countForward[pos] / totalFreq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    std::vector<double> countBackward(len + 1);
    {
        countBackward[len] = 1;
        for (int pos = len - 1; pos >= 0; pos--) {
            std::vector<int> suffix(seq.begin() + pos, seq.end());
            int sym = search(suffix, excludes);
            do {
                int symLen = get(sym).length();
                int freq = sym < freqs.size() ? freqs[sym] : 0;
                countBackward[pos] += freq * countBackward[pos + symLen] / totalFreq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    for (int pos = 0; pos < len; pos++) {
        std::vector<int> suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            int symLen = get(sym).length();
            int freq = sym < freqs.size() ? freqs[sym] : 0;
            double freqIncrement = freq / totalFreq * countForward[pos] * countBackward[pos + symLen];
            double v = multiplier * freqIncrement / countForward[len];
            if (result.count(sym) == 0) {
                result[sym] = 0;
            }
            result[sym] += v;
        }
        while ((sym = parent(sym)) >= 0);
    }
}

int IntDictImpl::search(const std::vector<int>& seq, const std::vector<int>& excludes = nullptr) const {
    int index = std::lower_bound(seqs_.begin(), seqs_.end(), seq) - seqs_.begin(); // TODO maybe it work in another way
    if (index >= 0) {
        if (excludes == nullptr || excludes.count(index) == 0)
            return index;
        index = -(parents_[index] + 2);
    }
    index = -(index + 2);
    while (index >= 0) {
        if (std::mismatch(seqs_[index].begin(), seqs_[index].end(), seq.begin()).first == seqs_[index].end() &&
            (excludes == nullptr || excludes.count(index) == 0))
            return index;
        index = parents_[index];
    }
    throw DICTIONARY_INDEX_IS_CORRUPTED_CODE; //TODO exceptions
}

int IntDictImpl::parse(const std::vector<int>& seq, std::vector<int>* output, const std::vector<int>& freqs, double totalFreq) {
    std::vector<int> result;
    double logProBab = weightedParse(seq, freqs, totalFreq, output);
    if (logProBab > 0 || debug) {
        std::cout << seq + " ->";
        for (int i = 0; i < output.size(); i++) {
            int symbol = output[i];
            if (symbol >= 0)
                std::cout << " " << get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " << logProBab << std::endl;
    }
    return output.size();
}

int IntDictImpl::parse(const std::vector<int>& seq, std::vector<int>* output, const std::unordered_set<int>& excludes = nullptr) {
    linearParse(seq, output, excludes);
    return builder.size();
}

std::vector<int> IntDictImpl::get(int index) const {
    return seqs_[index];
}

int IntDictImpl::size() const {
    return seqs_.size();
}

std::vector<std::vector<int>> IntDictImpl::alphabet() const {
    return std::vector<std::vector<int>>(seqs_);
}

int IntDictImpl::parent(int second) const {
    return parents_[second];
}

IntDictImpl::~IDictionary() {}