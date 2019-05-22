//
// Created by akhvorov on 01.07.18.
//

#include <iostream>
#include <algorithm>
#include <numeric>
#include "stat_dict_p.h"
#include "int_dict_p.h"
#include "fast_random.h"
#include "vector_hash.h"

StatDictImpl::StatDictImpl(double min_probability) {
    dict_ = std::shared_ptr<IntDict>(new IntDictImpl()); // Sanitizer: indirect leak
    symbol_freqs_ = IntSeq();
    parse_freqs_ = IntSeq();
    pairs_freqs_ = std::unordered_map<std::int64_t , int>(kAggPower); // Sanitizer: indirect leak
    min_probability_ = min_probability;
}

StatDictImpl::StatDictImpl(const std::vector<IntSeq>& seqs, double min_prob_result, IntSeq* init_freqs) {
    dict_ = std::make_shared<IntDictImpl>(seqs); // Sanitizer: indirect leak // replace int_dict to seqs
    symbol_freqs_ = IntSeq(static_cast<size_t>(seqs.size()));
    if (init_freqs == nullptr) {
        parse_freqs_ = IntSeq(static_cast<size_t>(dict_->size())); // Sanitizer: indirect leak
    } else {
        parse_freqs_ = IntSeq(*init_freqs);
        parse_freqs_init_power_ = std::accumulate(parse_freqs_.begin(), parse_freqs_.end(), 0.0);
        for (auto i = parse_freqs_.size(); i < dict_->size(); i++)
            parse_freqs_.push_back(0);
    }
    pairs_freqs_ = std::unordered_map<std::int64_t , int>(kAggPower); // Sanitizer: indirect leak
    min_probability_ = min_prob_result;
}

void StatDictImpl::update_symbol(int index, int freq) {
    if (index >= symbol_freqs_.size()) {
        for (auto i = symbol_freqs_.size(); i < index + 1; i++)
            symbol_freqs_.push_back(0); // Sanitizer: indirect leak
        for (auto i = parse_freqs_.size(); i < index + 1; i++)
            parse_freqs_.push_back(0); // Sanitizer: indirect leak
    }
    symbol_freqs_[index] += freq;
    parse_freqs_[index] += freq;
    power_ += freq;
}

int StatDictImpl::search(const IntSeq& seq, std::unordered_set<int>* excludes) const {
    return dict_->search(seq, excludes);
}

int StatDictImpl::search(const IntSeq& seq) const {
    return dict_->search(seq, nullptr);
}

const IntSeq& StatDictImpl::get(int index) const {
    return dict_->get(index);
}

int StatDictImpl::size() const {
    return dict_->size();
}

const std::vector<IntSeq>& StatDictImpl::alphabet() const {
    return dict_->alphabet();
}

int StatDictImpl::parent(int second) const {
    return dict_->parent(second);
}

int StatDictImpl::freq(int index) const {
    return index < symbol_freqs_.size() ? symbol_freqs_[index] : 0;
}

double StatDictImpl::code_length_per_char() const {
    double sum = 0;
    for (int i = 0; i < size(); i++) {
        int frequency = freq(i);
        if (frequency > 0)
            sum -= frequency * log(frequency);
    }
    return (sum + power_ * log(power_)) / total_chars_;
}

bool StatDictImpl::enough(double prob_found) const {
    return power_ > -log(prob_found) / min_probability_;
}

int StatDictImpl::parse(const IntSeq& seq, IntSeq* parse_result) {
    dict_->parse(seq, parse_freqs_, power_ + parse_freqs_init_power_, parse_result);
    total_chars_ += seq.size();
    int prev = -1;
    for (int symbol : *parse_result) {
        update_symbol(symbol, 1);
        if (prev >= 0)
            pairs_freqs_[(((std::int64_t) prev) << 32) | symbol] += 1; // Sanitizer: indirect leak
        prev = symbol;
    }
    return static_cast<int>(parse_result->size());
}

double StatDictImpl::expand(int slots, std::vector<IntSeq>* new_dict, IntSeq* freqs) const {
    std::vector<StatItem> items;
    std::unordered_set<IntSeq, VectorHash> known;
    for (const IntSeq& seq : alphabet()) {
        known.insert(seq);
        int symbol = search(seq);
        items.emplace_back(-1, symbol, std::numeric_limits<double>::max(), freq(symbol));
    }
    slots += alphabet().size();
    std::vector<double> start_with(symbol_freqs_.size());
    std::vector<double> ends_with(symbol_freqs_.size());
    for (auto &pairs_freq : pairs_freqs_) {
        std::int64_t code = pairs_freq.first;
        int freq = pairs_freq.second;
        auto first = static_cast<int>(code >> 32);
        auto second = static_cast<int>(code & 0xFFFFFFFFL);
        start_with[first] += freq;
        ends_with[second] += freq;
    }

    double total_pair_freqs = std::accumulate(start_with.begin(), start_with.end(), 0.0);
    for (auto &pairs_freq : pairs_freqs_) {
        std::int64_t code = pairs_freq.first;
        int freq = pairs_freq.second;
        auto first = static_cast<int>(code >> 32);
        auto second = static_cast<int>(code & 0xFFFFFFFFL);
        double ab = freq;
        double xb = ends_with[second] - freq;
        double ay = start_with[first] - freq;
        double xy = total_pair_freqs - ay - xb - ab;

        double params[] = {ab + 1, ay + 1, xb + 1, xy + 1};
        std::vector<double> dirichlet_params(std::begin(params), std::end(params));
        double score = 0;
        int samples_count = 10;
        std::vector<double> sample(dirichlet_params.size());
        for (int i = 0; i < samples_count; i++) {
            next_dirichlet(dirichlet_params, &sample);
            double pAB = sample[0];
            double pAY = sample[1];
            double pXB = sample[2];
            score += freq * pAB / (pAY + pAB) * log(pAB / (pAY + pAB) / (pXB + pAB)) / samples_count;
        }

        StatItem item(first, second, score, freq);
        IntSeq item_text;
        stat_item_to_text(item, &item_text);
        if (known.count(item_text) == 0) {
            known.insert(item_text);
            items.push_back(item);
        }
    }

    std::sort(items.begin(), items.end(), [] (const StatItem& a, const StatItem& b) { return -a.score() < -b.score(); });
    double min_prob_result = min_probability_;
    double accumulated_freqs = 0.0;
    for (const auto& pair : pairs_freqs_) {
        accumulated_freqs += pair.second;
    }

    for (const StatItem& item : items) {
        if (item.score() < 0)
            break;
        if (--slots < 0)
            break;
        IntSeq item_text;
        stat_item_to_text(item, &item_text);
        new_dict->push_back(item_text);
        freqs->push_back(item.count());
        if (item.first() >= 0)
            min_prob_result = std::min(min_prob_result, item.count() / accumulated_freqs);
    }
    return min_prob_result;
}

double StatDictImpl::reduce(int slots, std::vector<IntSeq>* new_dict, IntSeq* freqs) const {
    std::vector<StatItem> items;
    filter_stat_items(slots, &items);
    double power = 0.0;
    for (const auto& item : items)
        power += item.count();

    double min_prob_result = std::min(1. / size(), min_probability_);
    for (const auto& item : items) {
        double p = (item.count() + 1.0) / (power + size());
        if (parent(item.second()) >= 0)
            min_prob_result = std::min(p, min_prob_result);
        new_dict->push_back(get(item.second()));
        freqs->push_back(item.count());
    }
    return min_prob_result;
}

int StatDictImpl::filter_stat_items(int slots, std::vector<StatItem>* items) const {
    for (int s = 0; s < symbol_freqs_.size(); s++)
        if (parent(s) < 0)
            slots += 1;
    std::unordered_set<int> excludes;
    for (int id = 0; id < size(); id++)
        if (parent(id) >= 0 && freq(id) == 0)
            excludes.insert(id);
    stat_items(items, &excludes);
    while (items->size() > std::min((int)items->size(), slots))
        items->pop_back();
    return static_cast<int>(items->size());
}

int StatDictImpl::stat_items(std::vector<StatItem>* items, std::unordered_set<int>* excludes) const {
    double code_length = code_length_per_char() * total_chars_;
    for (int id = 0; id < symbol_freqs_.size(); id++) {
        if (excludes->count(id) == 0) {
            int count = freq(id);
            const IntSeq& seq = get(id);
            if (seq.size() > 1) {
                IntSeq parse;
                excludes->insert(id);
                weightedParse(seq, symbol_freqs_, power_, &parse, excludes); //TODO doesn't work, make base_dict
                excludes->erase(id);
                double new_power = power_ + (parse.size() - 1) * count;
                double code_length_without_symbol = code_length + count * log(count) -
                        power_ * log(power_) + new_power * log(new_power);
                for (int next : parse) {
                    int old_freq = freq(next);
                    int new_freq = old_freq + count;
                    code_length_without_symbol -= new_freq * log(new_freq) -
                            (old_freq > 0 ? old_freq * log(old_freq) : 0);
                }
                double score = code_length_without_symbol - code_length;
                if (score > 0) {
                    items->emplace_back(-1, id, score, count);
                }
            } else {
                items->emplace_back(-1, id, std::numeric_limits<double>::max(), count);
            }
        }
    }
    std::sort(items->begin(), items->end(), [] (const StatItem& a, const StatItem& b) { return -a.score() < -b.score(); });
    return static_cast<int>(items->size());
}

double StatDictImpl::weightedParse(const IntSeq& seq, const IntSeq& freqs, double total_freq,
                               IntSeq* result, std::unordered_set<int>* excludes) const {
    size_t len = seq.size();
    std::vector<double> score(len + 1, std::numeric_limits<double>::lowest());
    score[0] = 0;
    IntSeq symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        IntSeq suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = get(sym).size();
            double sym_log_prob = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(total_freq + size());
            if (score[sym_len + pos] < score[pos] + sym_log_prob)
            {
                score[sym_len + pos] = score[pos] + sym_log_prob;
                symbols[sym_len + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0);
    }
    IntSeq solution(len + 1);
    auto pos = static_cast<int>(len);
    int index = 0;
    while (pos > 0) {
        int sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym).size();
    }
    result->insert(result->end(), solution.begin() + len - index, solution.end() - 1);
    return score[len];
}

void StatDictImpl::stat_item_to_text(const StatItem& item, IntSeq* output) const {
    if (item.first() >= 0) {
        output->insert(output->end(), get(item.first()).begin(), get(item.first()).end());
        output->insert(output->end(), get(item.second()).begin(), get(item.second()).end());
    } else {
        output->insert(output->end(), get(item.second()).begin(), get(item.second()).end());
    }
}

//StatItem

StatItem::StatItem(int first, int second, double score, int count) {
    first_ = first;
    second_ = second;
    score_ = score;
    count_ = count;
}

bool StatItem::equals(const StatItem& stat_item) {
    return first_ == stat_item.first_ && second_ == stat_item.second_;
}

int StatItem::first() const {
    return first_;
}
int StatItem::second() const {
    return second_;
}
double StatItem::score() const {
    return score_;
}
int StatItem::count() const {
    return count_;
}
