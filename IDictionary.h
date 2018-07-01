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
    virtual int search(std::string seq) const = 0;
    virtual int search(std::string seq, std::vector<int> excludes) const = 0;

    virtual IntSeq parse(std::string seq, std::vector<int> freqs, double totalFreq) const = 0;
    virtual IntSeq parse(std::string seq) const = 0;
    virtual IntSeq parse(std::string seq, std::unordered_set<int> excludes) const = 0;

    virtual std::string get(int index) const = 0;

    virtual int size() const = 0;

    virtual std::vector<std::string> alphabet() const = 0;

    virtual int parent(int second) const = 0;

    virtual ~IDictionary() {}
};

#endif //VGRAM_IDICTIONARY_H
