//
// Created by akhvorov on 01.07.18.
//

#ifndef VGRAM_DICTEXPANSION_H
#define VGRAM_DICTEXPANSION_H

const double MAX_MIN_PROBABILITY = 0.002;
const double EXTENSION_FACTOR = 1.3;
const double MAX_POWER = 20000000;
const int AGG_POWER = 100000;



#endif //VGRAM_DICTEXPANSION_H


// not impl !!!
double DictExpansion::kl(std::vector<int> const & freqs, std::unordered_map<long, int> const & pairFreqs) {
    std::vector<double> freqXFirst(freqs.dim());
    for (auto e : pairFreqs) {
    std::int32_t code = e.first;
    std::int32_t freq = e.second;
    freqXFirst[(std::int32_t) (code >> 32)] = freq;
    }

    double totalPairFreqs = std::accumulate(std::begin(freqXFirst), std::end(freqXFirst), 0);
    double totalFreqs = std::accumulate(std::begin(freqs), std::end(freqs), 0);

    final double result[] = {0};
    pairFreqs.forEachEntry((code, freq) -> {
    final int first = (int) (code >>> 32);
    final int second = (int) (code & 0xFFFFFFFFL);
    final double pAB = freq / totalPairFreqs;
    final double pBcondA = freq / freqXFirst.get(first);
    final double pA = freqs.get(first) / totalFreqs;
    final double pB = freqs.get(second) / totalFreqs;
    result[0] += freq * pBcondA * log(pAB / pA / pB);
    return true;
    });
    return result[0];
}