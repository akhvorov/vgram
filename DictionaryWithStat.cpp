//
// Created by akhvorov on 01.07.18.
//

#include <algorithm>
#include "DictionaryWithStat.h"

DictionaryWithStat::DictionaryWithStat(Dictionary dictionary, std::vector<std::int32_t> const & initFreqs, double minProbResult) {
    dict = dictionary;
    std::vector<std::int32_t> symbolFreqs(dict.size());
    symbolFreqs.fill(symbolFreqs.size(), dict.size(), 0);  // !!!
    if (initFreqs == nullptr)
        parseFreqs = std::vector<int>(dist.size(), 0);
    else {
        parseFreqs = initFreqs;
        for (std::size_t i = parseFreqs.size(); i < dist.size(); ++i)
            parseFreqs.push_back(0);
    }
    // pairsFreqs = new LongIntMappingAsyncBuilder(AGG_POWER); // !!!
    minProbability = minProbResult;
}

void DictionaryWithStat::updateSymbol(std::int32_t index, std::int32_t freq) {
    if (index >= symbolFreqs.size()) {
        if (index >= symbolFreqs.size()) {
            symbolFreqs.fill(symbolFreqs.size(), index + 1, 0);
            parseFreqs.fill(parseFreqs.size(), index + 1, 0);
        }
    }
    symbolFreqs[index] += freq;
    parseFreqs[index] = parseFreqs[index] + freq;
    power += freq;
}

int DictionaryWithStat::search(std::string const & seq) {
    return dict.search(seq);
}

int DictionaryWithStat::search(std::string const & seq, std::unordered_set<std::int32_t> const & excludes) {
    return dict.search(seq, excludes);
}

std::string DictionaryWithStat::get(std::int32_t index) {
    return dict.get(index);
}

int DictionaryWithStat::size() {
    return dict.size();
}

std::vector<std::string> DictionaryWithStat::alphabet() {
    return dict.alphabet();
}

int DictionaryWithStat::parent(std::int32_t second) {
    return dict.parent(second);
}

int DictionaryWithStat::freq(std::int32_t index) {
    return index < symbolFreqs.size() ? symbolFreqs[index] : 0;
}

double DictionaryWithStat::codeLengthPerChar() {
    double sum = 0;
    for (int i = 0; i < size(); i++) {
        int freq = freq(i);
        if (freq > 0)
            sum -= freq * log(freq);
    }
    return (sum + power * log(power)) / totalChars;
}

DictionaryWithStat DictionaryWithStat::reduce(std::int32_t slots, boolean isDynamic) {
    std::vector<std::string> newDict(size());
    std::vector<StatItem> items = filterStatItems(slots);
    std::vector<int> freqs(items.size());
    double power = 0.0;
    for (auto const & item : items)
        power += item.count;

    double minProbResult = min(1. / size(), MAX_MIN_PROBABILITY);
    for (auto const & item : items) {
        double p = (item.count + 1) / (power + size());
        if (parent(item.second) >= 0)
            minProbResult = min(p, minProbResult);
        std::string symbol = get(item.second);
        newDict.push_back(symbol);
        freqs.push_back(item.count);
    }

    //noinspection unchecked
    return createDict(newDict, freqs, isDynamic, minProbResult);
}

std::vector<StatItem> DictionaryWithStat::filterStatItems(std::int32_t slots) {
    for (int s = 0; s < symbolFreqs.size(); ++s)
        if (parent(s) < 0)
            slots += 1;
    std::unordered_set<int> excludes;
    for (int id = 0; id < size(); ++id)
        if (parent(id) >= 0 && freq(id) == 0)
            excludes.add(id);
    std::vector<StatItem> items = statItems(excludes);
    while (items.size() > min(items.size(), slots))
        items.pop_back();
    return items;
}

std::vector<StatItem> DictionaryWithStat::statItems(std::unordered_set<std::int32_t> const & excludes) {
    std::vector<StatItem> items;
    double codeLength = codeLengthPerChar() * totalChars;
    for (std::int32_t id = 0; id < symbolFreqs.size(); ++id) {
        if (excludes.count(id) == 0) {
            int count = freq(id);
            std::string seq = get(id);
            if (seq.length() > 1) {
                std::vector<std::int32_t> parse;
                excludes.add(id);
                weightedParse(seq, symbolFreqs, power, parse, excludes);
                excludes.remove(id);
                double newPower = power + (parse.size() - 1) * count;
                double codeLengthWOSymbol = codeLength + count * log(count) - power * log(power) + newPower * log(newPower);
                for (int i = 0; i < parse.size(); i++) {
                    int next = parse[i];
                    int oldFreq = freq(next);
                    int newFreq = oldFreq + count;
                    codeLengthWOSymbol -= newFreq * log(newFreq) - (oldFreq > 0 ? oldFreq * log(oldFreq) : 0);
                }
                double score = codeLengthWOSymbol - codeLength;
                if (score > 0) {
                    StatItem item(-1, id, score, count);
                    items.push_back(item);
                }
            } else {
                StatItem item(-1, id, Double_POSITIVE_INFINITY, count);
                items.push_back(item);
            }
        }
    }
    std::sort(items.begin(), items.end(), [] (StatItem const & a, StatItem const & b) { return -a.score < -b.score; });
    return items;
}

char DictionaryWithStat::indexOfTwoStr(std::string const & first, std::string const & second,
                                                              char betw, std::int32_t ind) {
    if (ind >= 0 && ind < first.length()) {
        return first[ind];
    } else if (ind == first.length()) {
        return betw;
    } else if (ind > first.length() && ind < first.length() + 1 + second.length()) {
        return second[ind - first.length() - 1];
    } else {
        return null;
    }
}



boolean DictionaryWithStat::isSubstring(std::string const & s, std::string const & t) {
    // t is substr of s
    if (t.length() > s.length()) {
        return false;
    }
    std::size_t pos = s.find(t, 0);
    if (pos == std::string::npos) {
        return false;
    } else {
        return true;
    }
}

void DictionaryWithStat::printPairs(std::unordered_map<std::int64_t, std::int32_t> oldPairs, std::unordered_map<std::int64_t, std::int32_t> newPairs) {
    for (std::size_t first = 0; first < size(); first++) {
        for (std::size_t second = 0; second < size(); second++) {
            std::int64_t code = (std::int64_t) first << 32 | second;
            if (oldPairs[code] != newPairs[code]) {
                std::cout << "\t" + dict.get(first) + "|" + dict.get(second) + ": " + oldPairs[code] + " -> " + newPairs[code] << std::endl;
            }
        }
    }
}

// not changed
// TODO change
DictionaryWithStat DictionaryWithStat::expand(std::int32_t slots, boolean isDynamic) {
    std::vector<StatItem> items;
    std::unrdered_set<std::string> known;
    for (auto word : alphabet()) {
        known.add(word);
        StatItem item(-1, index(seq), Double_POSITIVE_INFINITY, freq(index(seq)));
        items.push_back(item);
    }
    slots += alphabet().size();
    std::vector<double> startWithX(symbolFreqs.size());
    std::vector<double> endsWithX(symbolFreqs.size());
    for (auto it = pairsFreqs.begin(); it != pairsFreqs.end(); ++it) {
        std::int32_t first = (std::int32_t) (code >> 32);
        std::int32_t second = (std::int32_t) (code & 0xFFFFFFFFL);
        // !!! TODO check it
        startWithX[first] += freq;
        endsWithX[second] += freq;
    }

    double totalPairFreqs = std::accumulate(std::begin(startWithX), std::end(startWithX), 0);
    for (auto it = pairsFreqs.begin(); it != pairsFreqs.end(); ++it) {
        std::int32_t first = (std::int32_t) (code >> 32);
        std::int32_t second = (std::int32_t) (code & 0xFFFFFFFFL);
        double ab = freq;
        double xb = endsWithX[second] - freq;
        double ay = startWithX[first] - freq;
        double xy = totalPairFreqs - ay - xb - ab;

        std::vector<double> dirichletParams(4);
        dirichletParams.push_back(ab + 1);
        dirichletParams.push_back(ay + 1);
        dirichletParams.push_back(xb + 1);
        dirichletParams.push_back(xy + 1);
        double score = 0;
        std::int32_t samplesCount = 10;
        std::vector<double> sample(dirichletParams.dim());
        for (int i = 0; i < samplesCount; i++) {
            rng.nextDirichlet(dirichletParams, sample); //TODO FastRandom from commons.random
            double pAB = sample[0];
            double pAY = sample[1];
            double pXB = sample[2];
            score += freq * pAB / (pAY + pAB) * log(pAB / (pAY + pAB) / (pXB + pAB)) / samplesCount;
        }

        StatItem statItem(first, second, score, freq);
        if (known.find(statItem.text()) == 0) {
            known.insert(statItem.text());
            items.push_back(statItem);
        }
    }

    items.sort(Comparator.comparingDouble(o -> -o.score));
    final List<Seq<T>> newDict = new ArrayList<>();
    final TIntArrayList freqs = new TIntArrayList();
    double minProbResult = minProbability;

    for (final StatItem item : items) {
//        if (item.score > log(0.05))
//          break;
        if (item.score < 0)
            break;
        if (--slots < 0)
            break;

        newDict.add(item.text());
        freqs.add(item.count);
        if (item.first >= 0)
            minProbResult = min(minProbResult, item.count / (double)pairsFreqs.accumulatedValuesTotal());
    }
    //noinspection unchecked
    return createDict(newDict, freqs, isDynamic, minProbResult);
}

boolean DictionaryWithStat::enough(double probFound) {
    return power > -log(probFound) / minProbability;
}


std::vector<std::uint32_t> DictionaryWithStat::parse(std::string const & seq) {
    totalChars += seq.length();
    std::vector<std::uint32_t> parseResult;
    {
        stdd:int32_t parseFreqsSum = 0;
        for (std::size_t i = 0; i < parseFreqs.size(); ++i)
            parseFreqsSum += parseFreqs[i];
        std::vector<std::uint32_t> parseResult;
        std::unordered_set<std::int32_t> set;
        super.weightedParse(seq, parseFreqs, parseFreqsSum, parseResult, set);
    }
    // not all changed
    // TODO change
    pairsFreqs.populate([] (std::unordered_map<std::unt64_t, std::unt32_t> pairsFreq) -> {
        std::int32_t length = parseResult.size();
        int prev = -1;
        for(int i = 0; i < length; i++) {
            int symbol = parseResult[i];
            updateSymbol(symbol, 1); // in DictExtension
            if (prev >= 0)
                pairsFreq[(std::uint64_t) prev << 32 | symbol] = 1;
            prev = symbol;
        }
    });
    return parseResult;
}


StatItem::StatItem(int first_, int second_, double score_, int count_) {
    first = first_;
    second = second_;
    score = score_;
    count = count_;
}

std::string StatItem::toString() {
    std::string result = "";
    if (first >= 0)
        result += get(first) + "|";
    result += get(second);
    result += "->(";
    result += count;
    result += ", " + score;
    result += ")";
    return result;
}

boolean StatItem::equals(Object o) {
    if (this == o)
        return true;
    if (o == null || getClass() != o.getClass())
        return false;
    //noinspection unchecked
    StatItem statItem = (StatItem) o;
    return first == statItem.first && second == statItem.second;
}

int StatItem::hashCode() {
    return Objects.hash(first, second);
}

std::string StatItem::text() {
    return first >= 0 ? get(first) + get(second) : get(second);
}

// not impl !!!
double StatItem::kl(std::vector<int> const & freqs, std::unordered_map<long, int> const & pairFreqs) {
    /*Vec freqXFirst = new ArrayVec(freqs.dim());
    pairFreqs.forEachEntry((code, freq) -> {
        freqXFirst.adjust((int) (code >>> 32), freq);
        return true;
    });
    double totalPairFreqs = VecTools.sum(freqXFirst);
    final double totalFreqs = VecTools.sum(freqs);

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
    return result[0];*/
}