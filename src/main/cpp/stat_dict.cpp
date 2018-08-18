//
// Created by akhvorov on 01.07.18.
//

#include <iostream>
#include <algorithm>
#include "stat_dict.h"

StatDict::StatDict(IntDict dictionary, const std::vector<int>& init_freqs, double min_prob_result) {
    dict_ = dictionary;
    symbol_freqs_ = std::vector<int>(dictionary.size());
    if (init_freqs == nullptr) {
        parse_freqs_ = std::vector<int>(dist.size(), 0);
    } else {
        parse_freqs_ = init_freqs;
        for (int i = parse_freqs_.size(); i < dist_.size(); i++)
            parse_freqs_.push_back(0);
    }
    pairs_freqs_ = std::unordered_map<long long, int>(kAggPower);
    min_probability_ = min_prob_result;
}

void StatDict::update_symbol(int index, int freq) {
    if (index >= symbol_freqs_.size()) {
        for (int i = symbol_freqs_.size(); i < index + 1; i++)
            symbol_freqs_.push_back(0);
        for (int i = parse_freqs_.size(); i < index + 1; i++)
            parse_freqs_.push_back(0);
    }
    symbol_freqs_[index] += freq;
    parse_freqs_[index] += freq;
    power += freq;
}

int StatDict::search(std::string const & seq) {
    return dict_.search(seq);
}

int StatDict::search(std::string const & seq, std::unordered_set<std::int32_t> const & excludes) {
    return dict_.search(seq, excludes);
}

std::vector<int> StatDict::get(std::int32_t index) {
    return dict_.get(index);
}

int StatDict::size() {
    return dict_.size();
}

std::vector<std::vector<int>> StatDict::alphabet() {
    return dict_.alphabet();
}

int StatDict::parent(int second) {
    return dict_.parent(second);
}

int StatDict::freq(int index) {
    return index < symbol_freqs_.size() ? symbol_freqs_[index] : 0;
}

double StatDict::code_length_per_char() {
    double sum = 0;
    for (int i = 0; i < size(); i++) {
        int freq = freq(i);
        if (freq > 0)
            sum -= freq * log(freq);
    }
    return (sum + power * log(power)) / total_chars_;
}

StatDict* StatDict::reduce(int slots) {
    std::vector<std::vector<int>> new_dict(size());
    std::vector<StatItem> items = filter_stat_items(slots);
    std::vector<int> freqs(items.size());
    double power = 0.0;
    for (const auto& item : items)
        power += item.count;

    double min_prob_result = min(1. / size(), kMaxMinProbability);
    for (const auto& item : items) {
        double p = (item.count + 1.0) / (power + size());
        if (parent(item.second) >= 0)
            min_prob_result = min(p, min_prob_result);
        std::vector<int> symbol = get(item.second);
        new_dict.push_back(symbol);
        freqs.push_back(item.count);
    }

    return new StatDict(new IntDict(new_dict), freqs, min_prob_result);
}

//TODO change return type to arg or something else
std::vector<StatItem> StatDict::filter_stat_items(int slots) {
    for (int s = 0; s < symbol_freqs_.size(); s++)
        if (parent(s) < 0)
            slots += 1;
    std::unordered_set<int> excludes;
    for (int id = 0; id < size(); id++)
        if (parent(id) >= 0 && freq(id) == 0)
            excludes.insert(id);
    std::vector<StatItem> items = stat_items(excludes); //TODO check it!
    while (items.size() > min(items.size(), slots))
        items.pop_back();
    return items;
}

std::vector<StatItem> StatDict::stat_items(const std::unordered_set<int>& excludes) {
    std::vector<StatItem> items;
    double code_length = code_length_per_char() * total_chars_;
    for (int id = 0; id < symbol_freqs_.size(); id++) {
        if (excludes.count(id) == 0) {
            int count = freq(id);
            std::vector<int> seq = get(id);
            if (seq.size() > 1) {
                std::vector<int> parse;
                excludes.insert(id);
                weightedParse(seq, symbolFreqs, power, parse, excludes); //TODO doesn't work, make base_dict
                excludes.remove(id);
                double new_power = power + (parse.size() - 1) * count;
                double code_length_without_symbol = codeL_length + count * log(count) - power * log(power) + new_power * log(new_power);
                for (int i = 0; i < parse.size(); i++) {
                    int next = parse[i];
                    int old_freq = freq(next);
                    int new_freq = old_freq + count;
                    code_length_without_symbol -= new_freq * log(new_freq) - (old_freq > 0 ? old_freq * log(old_freq) : 0);
                }
                double score = code_length_without_symbol - codeLength;
                if (score > 0) {
                    items.push_back(StatItem(-1, id, score, count));
                }
            } else {
                items.push_back(StatItem item(-1, id, std::numeric_limits<double>::max(), count));
            }
        }
    }
    std::sort(items.begin(), items.end(), [] (const StatItem& a, const StatItem& b) { return -a.score < -b.score; });
    return items;
}

int StatDict::index_of_two_str(const std::vector<int> &first, const std::vector<int> &second, int betw, int ind) {
    if (ind >= 0 && ind < first.size()) {
        return first[ind];
    } else if (ind == first.size()) {
        return betw;
    } else if (ind > first.size() && ind < first.size() + 1 + second.size()) {
        return second[ind - first.size() - 1];
    } else {
        return -1;
    }
}


bool StatDict::is_substring(const std::vector<int>& s, const std::vector<int>& t) {
    return std::search(s.begin(), s.end(), t.begin(), t.end()) != t.end();
}

void StatDict::print_pairs(const std::unordered_map<std::int64_t, int>& old_pairs,
                          const std::unordered_map<std::int64_t, int>& new_pairs) const {
    for (int first = 0; first < size(); first++) {
        for (int second = 0; second < size(); second++) {
            std::int64_t code = (std::int64_t) first << 32 | second;
            if (old_pairs[code] != new_pairs[code]) {
                std::cout << "\t" + dict_.get(first) + "|" + dict_.get(second) + ": " + old_pairs[code] + " -> " +
                             new_pairs[code] << std::endl;
            }
        }
    }
}

// some changes
// TODO change
StatDict* StatDict::expand(std::int32_t slots, boolean isDynamic) {
    std::vector<StatItem> items;
    std::unrdered_set<std::string> known;
    for (auto word : alphabet()) {
        known.add(word);
        items.push_back(StatItem(-1, index(seq), Double_POSITIVE_INFINITY, freq(index(seq))));
    }
    slots += alphabet().size();
    std::vector<double> startWithX(symbolFreqs.size());
    std::vector<double> endsWithX(symbolFreqs.size());
    for (auto it = pairsFreqs.begin(); it != pairsFreqs.end(); ++it) {
        std::int32_t code = it->first;
        std::int32_t freq = it->second;
        std::int32_t first = (std::int32_t) (code >> 32);
        std::int32_t second = (std::int32_t) (code & 0xFFFFFFFFL);
        // !!! TODO check it
        startWithX[first] += freq;
        endsWithX[second] += freq;
    }

    double totalPairFreqs = std::accumulate(startWithX.begin(), startWithX.end(), 0);
    for (auto it = pairsFreqs.begin(); it != pairsFreqs.end(); ++it) {
        std::int32_t code = it->first;
        std::int32_t freq = it->second;
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

    std::sort(items.begin(), items.end(), [] (StatItem const & a, StatItem const & b) { return -a.score < -b.score; });
    std::vector<std::string> newDict;
    std::vector<std::int32_t> freqs;
    double minProbResult = minProbability;

    for (StatItem item : items) {
        if (item.score < 0)
            break;
        if (--slots < 0)
            break;
        newDict.push_back(item.text());
        freqs.push_back(item.count);
        if (item.first >= 0)
            minProbResult = min(minProbResult, item.count / (double)pairsFreqs.accumulatedValuesTotal());
    }
    // TODO check this line, it's mistake
    return createDict(newDict, freqs, isDynamic, minProbResult); //TODO add ctor
}

boolean StatDict::enough(double probFound) {
    return power > -log(probFound) / minProbability;
}

void visitAssociations(int start, TIntDoubleProcedure procedure) {
    pairsFreqs.visitRange(((long) start) << 32, ((long) start + 1L) << 32, (a, b) -> procedure.execute((int)(a & 0x7FFFFFFFL), b));
}

std::vector<std::uint32_t> StatDict::parse(std::string const & seq) {
    totalChars += seq.length();
    std::vector<std::int32_t> parseResult;
    {
        std::unordered_set<std::int32_t> set;
        super.weightedParse(seq, parseFreqs, parseFreqsSum, parseResult, set);
    }
    // not all changed TODO impl populate
    pairsFreqs.populate([] (std::unordered_map<std::int64_t, std::int32_t> pairsFreq) -> {
        std::int32_t length = parseResult.size();
        std::int32_t prev = -1;
        for (std::size_t i = 0; i < length; ++i) {
            std::int32_t symbol = parseResult[i];
            updateSymbol(symbol, 1); // TODO in DictExtension
            if (prev >= 0)
                pairsFreq[(std::int64_t) prev << 32 | symbol] = 1; // TODO it's okay
            prev = symbol;
        }
    });
    return parseResult;
}


StatItem::StatItem(std::int32_t first_, std::int32_t second_, double score_, std::int32_t count_) {
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

boolean StatItem::equals(StatItem statItem) {
    return first == statItem.first && second == statItem.second;
}

// TODO change
int StatItem::hashCode() {
    return Objects.hash(first, second);
}

std::string StatItem::text() {
    return first >= 0 ? get(first) + get(second) : get(second);
}
