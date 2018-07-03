//
// Created by akhvorov on 01.07.18.
//

#include "DictionaryBase.h"

std::vector<int> DictionaryBase::parse(std::string const & seq, std::vector<int> const & freqs, double totalFreq) {
    std::vector<int> result;
    double logProBab = weightedParse(seq, freqs, totalFreq, result);
    if (logProBab > 0 || debug) {
        std::cout << seq + " ->";
        for (int i = 0; i < result.length(); i++) {
            final int symbol = result.intAt(i);
            if (symbol >= 0)
                std::cout << " " + get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " + logProBab << std::endl;
    }
    return result;
}

std::vector<int> DictionaryBase::parseEx(std::string const & seq, std::vector<int> const & freqs, double totalFreq) {
    std::vector<int> result;
    double logProBab = exhaustiveParse(seq, freqs, totalFreq, result, 0, Double.NEGATIVE_INFINITY);
    if (logProBab > 0 || debug) {
        std::cout << seq.toString() + " ->";
        for (int i = 0; i < result.length(); i++) {
            int symbol = result[i];
            if (symbol >= 0)
                std::cout << " " + get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " << std::endl;
    }
    return result;
}

std::vector<int> DictionaryBase::parse(std::string const & seq) {
    std::vector<int> builder;
    linearParse(seq, builder, nullptr);
    return builder;
}

std::vector<int> DictionaryBase::parse(std::string const & seq, std::unordered_set<int> const & excludes) {
    std::vector<int> builder;
    linearParse(seq, builder, excludes);
    return builder;
}

int DictionaryBase::linearParse(std::string const & seq, std::vector<int> const & builder,
                                std::unordered_set<int> const & excludes) {
    std::string suffix = seq;
    while (suffix.length() > 0) {
        int symbol;
        try {
            symbol = search(suffix, excludes);
            int symLen = get(symbol).length();
            suffix = suffix.substr(symLen, suffix.length() - symLen);
        }
        catch (int e_code) {
            if (DICTIONARY_INDEX_IS_CORRUPTED_CODE == e_code) {
                symbol = -1;
                suffix = suffix.substr(1, suffix.length() - 1);
            }
            else throw e_code;
        }
        builder.push_back(symbol);
    }
    return builder.length();
}

// my strange builder impl
double DictionaryBase::exhaustiveParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                                       std::vector<int> const & builder, double currentLogProbab, double bestLogProBab) {
    if (seq.length() == 0)
        return currentLogProbab;
    std::string suffix = seq;
    int symbol;

    int builderLen = builder.size();
    try {
        double bestProbability = Double_NEGATIVE_INFINITY;
        std::vector<int> bestSeq;
        symbol = search(suffix);

        do {
            builder.push_back(symbol);
            int symLen = get(symbol).length();
            std::string variant = suffix.substr(symLen, suffix.length() - symLen);
            double logProbability = currentLogProbab - log(totalFreq + freqs.size() + 1);
            logProbability += freqs.size() > symbol ? log(freqs[symbol] + 1) : 0.;
            if (logProbability > bestLogProBab) {
                if (variant.length() > 0)
                    logProbability = exhaustiveParse(variant, freqs, totalFreq, builder, logProbability, bestLogProBab);
                if (logProbability > bestProbability) {
                    bestProbability = logProbability;
                    bestSeq.clear();
                    for (std::size_t i = 0; i < builder.size(); ++i) {
                        bestSeq.push_back(builder[i]);
                    }
                }
            }
            while (builder.size() > builderLen) {
                builder.pop_back();
            }
        }
        while ((symbol = parent(symbol)) >= 0);
        for (std::size_t i = 0; i < bestSeq.size(); ++i) {
            builder.push_back(bestSeq[i]);
        }
        return bestProbability;
    }
    catch (int e_code) {
        if (DICTIONARY_INDEX_IS_CORRUPTED_CODE == e_code) {
            suffix = suffix.substr(1, suffix.length() - 1);
            builder.append(-1);
            return exhaustiveParse(suffix, freqs, totalFreq, builder, currentLogProbab - 1e-5, bestLogProBab);
        }
        else throw e_code;
    }
}

double DictionaryBase::weightedParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                                     std::vector<int> const & builder) {
    return weightedParse(seq, freqs, totalFreq, builder, nullptr);
}

double DictionaryBase::weightedParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                                     std::vector<int> const & builder, std::unordered_set<int> const & excludes)
{
    int len = seq.length();
    std::vector<double> score(len + 1, Double_NEGATIVE_INFINITY);
    score[0] = 0;
    std::vector<int> symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        std::string suffix = seq.substr(pos, len - pos);
        int sym = search(suffix, excludes);
        do {
            int symLen = get(sym).length();
            double symLogProb = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(totalFreq + size());

            if (score[symLen + pos] < score[pos] + symLogProb)
            {
                score[symLen + pos] = score[pos] + symLogProb;
                symbols[symLen + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0); // !!!
    }
    std::vector<int> solution(len + 1);
    int pos = len;
    int index = 0;
    while (pos > 0) {
        int sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym).length();
    }
    for (int i = 0; i < index; i++) {
        builder.push_back(solution[len - index + i]);
    }
    return score[len];
}

void DictionaryBase::weightParseVariants(std::string const & seq, double multiplier, std::vector<int> const & freqs,
                                         double totalFreq, std::unordered_set<int> const & excludes,
                                         std::unordered_map<int, double> const & result)
{
    int len = seq.length();
    std::vector<double> countForward(len + 1);
    {
        countForward[0] = 1;
        for (int pos = 0; pos < len; pos++) {
            std::string suffix = seq.substr(pos, len - pos);
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
            std::string suffix = seq.substr(pos, len - pos);
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
        std::string suffix = seq.substr(pos, len - pos);
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
            //result.adjustOrPutValue(sym, v, v);
        }
        while ((sym = parent(sym)) >= 0);
    }
}

int DictionaryBase::search(std::string seq) {
    return search(seq, nullptr);
}