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
const double MAX_MIN_PROBABILITY = 0; //TODO change

struct DictionaryWithStat : DictionaryBase
{
private:
    Dictionary<T> dict;
    std::vector<std::int32_t> symbolFreqs;
    std::vector<std::int32_t> parseFreqs;
    double power = 0;
    //LongIntMappingAsyncBuilder pairsFreqs;
    std::unordered_map<std::int64, std::int64> pairsFreqs;
    double minProbability;
    double totalChars = 0;
    //FastRandom rng = new FastRandom(0);

};

class StatItem {
    std::int32_t first;
    std::int32_t second;
    double score;
    std::int32_t count;

};

#endif //VGRAM_DICTIONARYWITHSTAT_H
