//
// Created by akhvorov on 01.07.18.
//

#include <iostream>
#include <algorithm>
#include "stat_dict.h"

StatDict::StatDict(const IntDict& dictionary, const std::vector<int>& init_freqs, double min_prob_result) {
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
    power_ += freq;
}

int StatDict::search(const std::vector<int>& seq) {
    return dict_.search(seq);
}

int StatDict::search(const std::vector<int>& seq, const std::unordered_set<int>& excludes) {
    return dict_.search(seq, excludes);
}

std::vector<int>* StatDict::get(int index) {
    return dict_.get(index);
}

int StatDict::size() {
    return dict_.size();
}

std::vector<std::vector<int>>* StatDict::alphabet() {
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

//TODO change types and args
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
                items.push_back(StatItem(-1, id, std::numeric_limits<double>::max(), count));
            }
        }
    }
    std::sort(items.begin(), items.end(), [] (const StatItem& a, const StatItem& b) { return -a.score < -b.score; });
    return items;
}

int StatDict::index_of_two_str(const std::vector<int>& first, const std::vector<int>& second, int betw, int ind) {
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

// TODO change
StatDict* StatDict::expand(int slots) {
    std::vector<StatItem> items;
    std::unrdered_set<std::vector<int>> known;
    for (const auto& seq : alphabet()) {
        known.add(seq);
        items.push_back(StatItem(-1, search(seq), std::numeric_limits<double>::max(), freq(search(seq))));
    }
    slots += alphabet().size();
    std::vector<double> start_with(symbol_freqs_.size());
    std::vector<double> ends_with(symbol_freqs_.size());
    for (auto it = pairsFreqs.begin(); it != pairsFreqs.end(); it++) {
        int code = it->first;
        int freq = it->second;
        int first = code >> 32;
        int second = code & 0xFFFFFFFFL;
        // !!! TODO check it
        start_with[first] += freq;
        ends_with[second] += freq;
    }

    double total_pair_freqs = std::accumulate(start_with.begin(), start_with.end(), 0);
    for (auto it = pairs_freqs_.begin(); it != pairs_freqs_.end(); it++) {
        int code = it->first;
        int freq = it->second;
        int first = code >> 32;
        int second = code & 0xFFFFFFFFL;
        double ab = freq;
        double xb = ends_with[second] - freq;
        double ay = start_with[first] - freq;
        double xy = total_pair_freqs - ay - xb - ab;

        std::vector<double> dirichlet_params(4);
        dirichlet_params.push_back(ab + 1);
        dirichlet_params.push_back(ay + 1);
        dirichlet_params.push_back(xb + 1);
        dirichlet_params.push_back(xy + 1);
        double score = 0;
        int samples_count = 10;
        std::vector<double> sample(dirichlet_params.size());
        for (int i = 0; i < samples_count; i++) {
            rng.nextDirichlet(dirichlet_params, sample); //TODO FastRandom from commons.random
            double pAB = sample[0];
            double pAY = sample[1];
            double pXB = sample[2];
            score += freq * pAB / (pAY + pAB) * log(pAB / (pAY + pAB) / (pXB + pAB)) / samples_count;
        }

        StatItem item(first, second, score, freq);
        if (known.find(item.text()) == 0) {
            known.insert(item.text());
            items.push_back(item);
        }
    }

    std::sort(items.begin(), items.end(), [] (const StatItem& a, const StatItem& b) { return -a.score < -b.score; });
    std::vector<std::vector<int>> new_dict;
    std::vector<int> freqs;
    double min_prob_result = min_probability_;

    for (const StatItem& item : items) {
        if (item.score < 0)
            break;
        if (--slots < 0)
            break;
        new_dict.push_back(item.text());
        freqs.push_back(item.count);
        if (item.first >= 0)
            min_prob_result = min(min_prob_result, item.count / (double) pairs_freqs_.accumulatedValuesTotal()); //TODO change it
    }
    // TODO check this line, it's mistake
    return StatDict(IntDict(new_dict), freqs, min_prob_result);
}

bool StatDict::enough(double prob_found) {
    return power > -log(prob_found) / min_probability_;
}

//void visitAssociations(int start, TIntDoubleProcedure procedure) {
//    pairsFreqs.visitRange(((long) start) << 32, ((long) start + 1L) << 32, (a, b) -> procedure.execute((int)(a & 0x7FFFFFFFL), b));
//}

int StatDict::parse(const std::vector<int>& seq, std::vector<int>* parse_result) {
    total_chars_ += seq.size();
    {
        std::unordered_set<int> set;
        //TODO !!! super is dict_ ?
        super.weightedParse(seq, parse_freqs_, std::accumulate(parse_freqs_.begin(), parse_freqs_.end(), 0),
                            parse_result, set);
    }
    int length = parse_result.size();
    int prev = -1;
    for (int i = 0; i < length; ++i) {
        int symbol = parse_result[i];
        updateSymbol(symbol, 1); // TODO in DictExtension
        if (prev >= 0)
            pairs_freqs_[(std::int64_t) prev << 32 | symbol] += 1;
        prev = symbol;
    }
    return parse_result.size();
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

bool StatItem::equals(const StatItem& stat_item) {
    return first == stat_item.first && second == stat_item.second;
}

std::vector<int>* StatItem::text() {
    if (first >= 0) {
        std::vector<int> result(get(first));
        std::vector<int> conc(get(second));
        result.insert(result.end(), conc.begin(), conc.end());
        return result;
    } else {
        return get(second);
    }
}
