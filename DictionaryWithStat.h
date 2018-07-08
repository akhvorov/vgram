//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYWITHSTAT_H
#define VGRAM_DICTIONARYWITHSTAT_H

#include <cmath>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "DictExpansion.h"

//#define std::int32_t int32
//#define std::int64_t int64
//#define std::vector<std::int32_t> int_vec

const double Double_POSITIVE_INFINITY = 2e123;
const double MAX_MIN_PROBABILITY = 0.002; //TODO change

struct DictionaryWithStat : DictionaryBase
{
private:
    Dictionary<T> dict;
    std::vector<std::int32_t> symbolFreqs;
    std::vector<std::int32_t> parseFreqs;
    double power = 0;
    std::unordered_map<std::int64, std::int64> pairsFreqs;
    double minProbability;
    double totalChars = 0;
    //FastRandom rng = new FastRandom(0);

    DictionaryWithStat(Dictionary dictionary, std::vector<std::int32_t> const & initFreqs, double minProbResult);

    void updateSymbol(std::int32_t index, std::int32_t freq);
    std::int32_t search(std::string const & seq);
    std::int32_t search(std::string const & seq, std::unordered_set<std::int32_t> const & excludes);
    std::string get(std::int32_t index);
    std::size_t size();
    std::vector<std::string> alphabet();
    std::int32_t parent(std::int32_t second);
    std::int32_t freq(std::int32_t index);
    double codeLengthPerChar();

    DictionaryWithStat reduce(std::int32_t slots, boolean isDynamic);
    std::vector<StatItem> filterStatItems(std::int32_t slots);
    std::vector<StatItem> statItems(std::unordered_set<std::int32_t> const & excludes);

    char indexOfTwoStr(std::string const & first, std::string const & second,
                                           char betw, std::int32_t ind);
    boolean isSubstring(std::string const & s, std::string const & t);

    void printPairs(std::unordered_map<std::int64_t, std::int32_t> oldPairs,
                                        std::unordered_map<std::int64_t, std::int32_t> newPairs);

// TODO change
    DictionaryWithStat expand(std::int32_t slots, boolean isDynamic);
    boolean enough(double probFound);
    std::vector<std::uint32_t> parse(std::string const & seq);

};

class StatItem {

private:
    std::int32_t first;
    std::int32_t second;
    double score;
    std::int32_t count;

public:
    StatItem::StatItem(std::int32_t first_, std::int32_t second_, double score_, std::int32_t count_);
    std::string StatItem::toString();
    boolean StatItem::equals(StatItem statItem);

    // TODO change
    int StatItem::hashCode();
    std::string StatItem::text();
};

#endif //VGRAM_DICTIONARYWITHSTAT_H
