//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYBASE_H
#define VGRAM_DICTIONARYBASE_H

#include "IDictionary.h"
#import <vector>
#import <unordered_set>

const double Double_NEGATIVE_INFINITY = -1e122;
const std::string DICTIONARY_INDEX_IS_CORRUPTED = "Dictionary index is corrupted!";
const int DICTIONARY_INDEX_IS_CORRUPTED_CODE = 1234;

struct DictionaryBase : IDictionary
{
private:
    bool debug = false;
public:
    DictionaryBase();
    std::vector<int> parse(std::string const & seq, std::vector<int> const & freqs, double totalFreq);
    std::vector<int> parseEx(std::string const & seq, std::vector<int> const & freqs, double totalFreq);
    std::vector<int> parse(std::string const & seq);
    std::vector<int> parse(std::string const & seq, std::unordered_set<int> const & excludes);
    int linearParse(std::stringseq, std::vector<int> const & builder, std::unordered_set<int> const & excludes);

    double exhaustiveParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                           std::vector<int> const & builder, double currentLogProbab, double bestLogProBab);

    double weightedParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                         std::vector<int> const & builder);

    double weightedParse(std::string const & seq, std::vector<int> const & freqs, double totalFreq,
                         std::vector<int> const & builder, std::unordered_set<int> const & excludes);

    void weightParseVariants(std::string const & seq, double multiplier, std::vector<int> const & freqs, double totalFreq,
                             std::unordered_set<int> const & excludes, std::unordered_map<int, double> const & result);

    int search(std::string const & seq);
};

#endif //VGRAM_DICTIONARYBASE_H
