//
// Created by akhvorov on 01.07.18.
//

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

DictionaryWithStat DictionaryWithStat::expand(std::int32_t slots, boolean isDynamic) {
    std::vector<StatItem> items;
    std::unrdered_set<std::string> known;
    // !!!
    alphabet().stream().peek(known::add).map(seq -> new StatItem(-1, index(seq), Double.POSITIVE_INFINITY, freq(index(seq)))).forEach(items::add);
    slots += alphabet().size();
    Vec startWithX = new ArrayVec(symbolFreqs.size());
    Vec endsWithX = new ArrayVec(symbolFreqs.size());
    pairsFreqs.visit((code, freq) -> {
        final int first = (int) (code >>> 32);
        final int second = (int) (code & 0xFFFFFFFFL);

        startWithX.adjust(first, freq);
        endsWithX.adjust(second, freq);
        return true;
    });

    final double totalPairFreqs = VecTools.sum(startWithX);
    pairsFreqs.visit((code, freq) -> {
        final int first = (int) (code >>> 32);
        final int second = (int) (code & 0xFFFFFFFFL);

//        final double pairProbIndependentDirichlet = freq(first) * freq(second) / power / power;
//        final double lambda = pairsFreqs.accumulatedValuesTotal() * pairProbIndependentDirichlet;
//        final double score = MathTools.logPoissonProbability(lambda, freq);

//        final double pAB = freq / totalPairFreqs;
////        final double pBcondA = (freq + 1) / (freqXFirst.get(first) + symbolFreqs.size() - 1);
//        final double pBcondA = freq / startWithX.get(first);
//        double freqA = symbolFreqs.get(first);
//        final double pA = freqA / power;
//        double freqB = symbolFreqs.get(second);
//        final double pB = freqB / power;
//        double score = freq * pBcondA * log(pAB / pA / pB);
//        if (Math.min(freqA, freqB) < 2)
//          score = -1;

        final double ab = freq;
        final double xb = endsWithX.get(second) - freq;
        final double ay = startWithX.get(first) - freq;
        final double xy = totalPairFreqs - ay - xb - ab;

        final Vec dirichletParams = new ArrayVec(ab + 1, ay + 1, xb + 1, xy + 1);
        double score = 0;
        int samplesCount = 10;
        Vec sample = new ArrayVec(dirichletParams.dim());
        for (int i = 0; i < samplesCount; i++) {
            rng.nextDirichlet(dirichletParams, sample);
            double pAB = sample.get(0);
            double pAY = sample.get(1);
            double pXB = sample.get(2);
            score += freq * pAB / (pAY + pAB) * log(pAB / (pAY + pAB) / (pXB + pAB)) / samplesCount;
        }

        final StatItem statItem = new StatItem(first, second, score, freq);
        if (!known.contains(statItem.text())) {
            known.add(statItem.text());
            items.add(statItem);
        }
        return true;
    });

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

//    static long counter = 0;
IntSeq DictionaryWithStat::parse(Seq<T> seq) {
    totalChars += seq.length();
    final IntSeq parseResult;
//      boolean debug = ++counter % 10000 == 0;
    {
        IntSeqBuilder builder = new IntSeqBuilder();
        super.weightedParse(seq, parseFreqs, parseFreqs.sum(), builder, new TIntHashSet());
        parseResult = builder.build();
//        if (debug)
//          System.out.println(parseResult.stream().mapToObj(this::get).map(Object::toString).collect(Collectors.joining("|")));
    }
    pairsFreqs.populate(pairsFreq -> {
        final int length = parseResult.length();
        int prev = -1;
        for(int i = 0; i < length; i++) {
            final int symbol = parseResult.intAt(i);
            updateSymbol(symbol, 1);
            if (prev >= 0)
                pairsFreq.adjustOrPutValue((long) prev << 32 | symbol, 1, 1);
            prev = symbol;
        }
    });
    return parseResult;
}


StatItem::StatItem(int first, int second, double score, int count) {
    this.first = first;
    this.second = second;
    this.score = score;
    this.count = count;
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