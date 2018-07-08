//
// Created by akhvorov on 01.07.18.
//

#include "DictionaryBase.h"

DictionaryBase::DictionaryBase() {

}

std::vector<std::int32_t> DictionaryBase::parse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq) {
    std::vector<std::int32_t> result;
    double logProBab = weightedParse(seq, freqs, totalFreq, result);
    if (logProBab > 0 || debug) {
        std::cout << seq + " ->";
        for (std::size_t i = 0; i < result.length(); i++) {
            std::int32_t symbol = result[i];
            if (symbol >= 0)
                std::cout << " " + get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " + logProBab << std::endl;
    }
    return result;
}

std::vector<std::int32_t> DictionaryBase::parseEx(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq) {
    std::vector<std::int32_t> result;
    double logProBab = exhaustiveParse(seq, freqs, totalFreq, result, 0, Double_NEGATIVE_INFINITY);
    if (logProBab > 0 || debug) {
        std::cout << seq.toString() + " ->";
        for (std::size_t i = 0; i < result.length(); i++) {
            std::int32_t symbol = result[i];
            if (symbol >= 0)
                std::cout << " " + get(symbol);
            else
                std::cout << "##unknown##";
        }
        std::cout << " " << std::endl;
    }
    return result;
}

std::vector<std::int32_t> DictionaryBase::parse(std::string const & seq) {
    std::vector<std::int32_t> builder;
    linearParse(seq, builder, nullptr);
    return builder;
}

std::vector<std::int32_t> DictionaryBase::parse(std::string const & seq, std::unordered_set<std::int32_t> const & excludes) {
    std::vector<std::int32_t> builder;
    linearParse(seq, builder, excludes);
    return builder;
}

std::size_t DictionaryBase::linearParse(std::string const & seq, std::vector<std::int32_t> const & builder,
                                std::unordered_set<std::int32_t> const & excludes) {
    std::string suffix = seq;
    while (suffix.length() > 0) {
        std::int32_t symbol;
        try {
            symbol = search(suffix, excludes);
            std::int32_t symLen = get(symbol).length();
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
    return builder.size();
}

// my strange builder impl
double DictionaryBase::exhaustiveParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                                       std::vector<std::int32_t> const & builder, double currentLogProbab, double bestLogProBab) {
    if (seq.length() == 0)
        return currentLogProbab;
    std::string suffix = seq;
    std::int32_t symbol;

    std::size_t builderLen = builder.size();
    try {
        double bestProbability = Double_NEGATIVE_INFINITY;
        std::vector<std::int32_t> bestSeq;
        symbol = search(suffix);

        do {
            builder.push_back(symbol);
            std::int32_t symLen = get(symbol).length();
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

double DictionaryBase::weightedParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                                     std::vector<std::int32_t> const & builder) {
    return weightedParse(seq, freqs, totalFreq, builder, nullptr);
}

double DictionaryBase::weightedParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                                     std::vector<std::int32_t> const & builder, std::unordered_set<std::int32_t> const & excludes)
{
    std::size_t len = seq.length();
    std::vector<double> score(len + 1, Double_NEGATIVE_INFINITY);
    score[0] = 0;
    std::vector<std::int32_t> symbols(len + 1);

    for (std::size_t pos = 0; pos < len; pos++) {
        std::string suffix = seq.substr(pos, len - pos);
        std::int32_t sym = search(suffix, excludes);
        do {
            std::int32_t symLen = get(sym).length();
            double symLogProb = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(totalFreq + size());

            if (score[symLen + pos] < score[pos] + symLogProb)
            {
                score[symLen + pos] = score[pos] + symLogProb;
                symbols[symLen + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0); // !!!
    }
    std::vector<std::int32_t> solution(len + 1);
    std::size_t pos = len;
    std::size_t index = 0;
    while (pos > 0) {
        std::int32_t sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym).length();
    }
    for (std::size_t i = 0; i < index; i++) {
        builder.push_back(solution[len - index + i]);
    }
    return score[len];
}

void DictionaryBase::weightParseVariants(std::string const & seq, double multiplier, std::vector<std::int32_t> const & freqs,
                                         double totalFreq, std::unordered_set<std::int32_t> const & excludes,
                                         std::unordered_map<std::int32_t, double> const & result)
{
    std::int32_t len = seq.length();
    std::vector<double> countForward(len + 1);
    {
        countForward[0] = 1;
        for (std::size_t pos = 0; pos < len; pos++) {
            std::string suffix = seq.substr(pos, len - pos);
            std::int32_t sym = search(suffix, excludes);
            do {
                std::int32_t symLen = get(sym).length();
                std::int32_t freq = sym < freqs.size() ? freqs[sym] : 0;
                countForward[pos + symLen] += freq * countForward[pos] / totalFreq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    std::vector<double> countBackward(len + 1);
    {
        countBackward[len] = 1;
        for (std::size_t pos = len - 1; pos >= 0; pos--) {
            std::string suffix = seq.substr(pos, len - pos);
            int sym = search(suffix, excludes);
            do {
                std::int32_t symLen = get(sym).length();
                std::int32_t freq = sym < freqs.size() ? freqs[sym] : 0;
                countBackward[pos] += freq * countBackward[pos + symLen] / totalFreq;
            }
            while ((sym = parent(sym)) >= 0);
        }
    }

    for (std::size_t pos = 0; pos < len; pos++) {
        std::string suffix = seq.substr(pos, len - pos);
        std::int32_t sym = search(suffix, excludes);
        do {
            std::int32_t symLen = get(sym).length();
            std::int32_t freq = sym < freqs.size() ? freqs[sym] : 0;
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

std::int32_t DictionaryBase::search(std::string seq) {
    return search(seq, nullptr);
}