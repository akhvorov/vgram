//
// Created by akhvorov on 01.07.18.
//

#include <vector>
#include <unordered_set>

#ifndef VGRAM_IDICTIONARY_H
#define VGRAM_IDICTIONARY_H

struct IDictionary
{
public:
    virtual std::int32_t search(std::string seq) const = 0;
    virtual std::int32_t search(std::string seq, std::vector<std::int32_t> excludes) const = 0;

    virtual std::vector<std::int32_t> parse(std::string seq, std::vector<std::int32_t> freqs, double totalFreq) const = 0;
    virtual std::vector<std::int32_t> parse(std::string seq) const = 0;
    virtual std::vector<std::int32_t> parse(std::string seq, std::unordered_set<std::int32_t> excludes) const = 0;

    virtual std::string get(std::int32_t index) const = 0;

    virtual std::size_t size() const = 0;

    virtual std::vector<std::string> alphabet() const = 0;

    virtual std::int32_t parent(std::int32_t second) const = 0;

    virtual ~IDictionary() {}
};

#endif //VGRAM_IDICTIONARY_H
