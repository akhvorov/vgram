//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTEXPANSION_H
#define VGRAM_DICTEXPANSION_H

#include "DictionaryWithStat.h"

/*const double MAX_MIN_PROBABILITY = 0.002;
const double EXTENSION_FACTOR = 1.3;
const double MAX_POWER = 20000000;
const int AGG_POWER = 100000;*/


class DictExpansion
{
public:
    const double EXTENSION_FACTOR = 1.3;
    const double MAX_POWER = 20000000;
    const double MAX_MIN_PROBABILITY = 0.002;
    const std::int32_t AGG_POWER = 100000;


    DictExpansion(Collection<T> alphabet, final int size);
// TODO remove ?
    DictExpansion(Collection<T> alphabet, std::size_t size, boolean trace);
// TODO remove ?
    DictExpansion(Collection<T> alphabet, std::size_t size, PrintStream trace);
    DictExpansion(IDictionary alphabet, std::size_t size);
    DictExpansion(IDictionary alphabet, std::size_t size, PrintStream trace);
    DictExpansion(std::int32_t slots);
    DictExpansion(int size, PrintStream trace);

    DictionaryWithStat createDict(std::vector<std::string> alphabet, std::vector<std::int32_t> initFreqs,
                                                 boolean isDynamic, double minProbResult);

    IDictionary result();
    IDictionary alpha();
    void accept(std::string seq);
    void update();
    std::vector<std::int32_t> resultFreqs();
    void printPairs(std::ofstream ps);
    void print(std::ofstream fileWriter);
    double codeLength();
    double kl(std::vector<int> const & freqs, std::unordered_map<long, int> const & pairFreqs);

private:
    boolean isDynamic;
    std::size_t size;
    PrintStream trace;
    IDictionary initial;
    DictionaryWithStat current;
    boolean populate = true;
    DictionaryWithStat result;

    std::size_t alphabetSize;
    double probFound = 0.1;
    double bestCompressionRate = 1;
    std::int32_t noRateIncreaseTurns = 0;
};


#endif //VGRAM_DICTEXPANSION_H
