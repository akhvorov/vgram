//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTIONARYBASE_H
#define VGRAM_DICTIONARYBASE_H

#include "IDictionary.h"
#import <vector>
#import <unordered_set>
#import <unordered_map>

const double Double_NEGATIVE_INFINITY = -1e122;
const std::string DICTIONARY_INDEX_IS_CORRUPTED = "Dictionary index is corrupted!";
const int DICTIONARY_INDEX_IS_CORRUPTED_CODE = 1234;

struct DictionaryBase : IDictionary
{
private:
    bool debug = false;
public:
    DictionaryBase();
    std::vector<std::int32_t> parse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq);
    std::vector<std::int32_t> parseEx(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq);
    std::vector<std::int32_t> parse(std::string const & seq);
    std::vector<std::int32_t> parse(std::string const & seq, std::unordered_set<std::int32_t> const & excludes);
    std::size_t linearParse(std::string const & seq, std::vector<std::int32_t> & builder, std::unordered_set<int> const & excludes);

    double exhaustiveParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                           std::vector<std::int32_t> & builder, double currentLogProbab, double bestLogProBab);

    double weightedParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                         std::vector<std::int32_t> & builder);

    double weightedParse(std::string const & seq, std::vector<std::int32_t> const & freqs, double totalFreq,
                         std::vector<std::int32_t> & builder, std::unordered_set<std::int32_t> const & excludes);

    void weightParseVariants(std::string const & seq, double multiplier, std::vector<std::int32_t> const & freqs, double totalFreq,
                             std::unordered_set<std::int32_t> const & excludes, std::unordered_map<std::int32_t, double> & result);

    std::int32_t search(std::string const & seq);
};

#endif //VGRAM_DICTIONARYBASE_H
