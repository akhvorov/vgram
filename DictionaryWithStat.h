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

const Double_POSITIVE_INFINITY = 2e123;

struct DictionaryWithStat : DictionaryBase
{
private:
    Dictionary<T> dict;
    std::vector<int> symbolFreqs;
    std::vector<int> parseFreqs;
    double power = 0;
    //LongIntMappingAsyncBuilder pairsFreqs;
    double minProbability;
    double totalChars = 0;
    //FastRandom rng = new FastRandom(0);

};

class StatItem {
    int first;
    int second;
    double score;
    int count;

};

#endif //VGRAM_DICTIONARYWITHSTAT_H
