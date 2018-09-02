//
// Created by akhvorov on 01.07.18.
//

#include <iostream>
#include <algorithm>
#include <numeric>
#include <memory>
#include "stat_dict.h"
#include "vector_hash.h"

StatDict::StatDict(const std::vector<std::vector<int>>& seqs, double min_prob_result, std::vector<int>* init_freqs) {
    IntDictImpl int_dict(seqs);
    dict_ = std::make_shared<IntDictImpl>(int_dict);
    dict_ = std::make_shared<IntDictImpl>(int_dict);
    symbol_freqs_ = std::vector<int>(int_dict.size());
    if (init_freqs == nullptr) {
        parse_freqs_ = std::vector<int>(dict_->size());
    } else {
        parse_freqs_ = std::vector<int>(*init_freqs);
        parse_freqs_init_power_ = std::accumulate(parse_freqs_.begin(), parse_freqs_.end(), 0.0);
        for (auto i = parse_freqs_.size(); i < dict_->size(); i++)
            parse_freqs_.push_back(0);
    }
    pairs_freqs_ = std::unordered_map<std::int64_t , int>(kAggPower);
    min_probability_ = min_prob_result;
}

void StatDict::update_symbol(int index, int freq) {
    if (index >= symbol_freqs_.size()) {
        for (auto i = symbol_freqs_.size(); i < index + 1; i++)
            symbol_freqs_.push_back(0);
        for (auto i = parse_freqs_.size(); i < index + 1; i++)
            parse_freqs_.push_back(0);
    }
    symbol_freqs_[index] += freq;
    parse_freqs_[index] += freq;
    power_ += freq;
}

int StatDict::search(const std::vector<int>& seq, std::unordered_set<int>* excludes) const {
    return dict_->search(seq, excludes);
}

const std::vector<int>* StatDict::get(int index) const {
    return dict_->get(index);
}

int StatDict::size() const {
    return dict_->size();
}

const std::vector<std::vector<int>>* StatDict::alphabet() const {
    return dict_->alphabet();
}

int StatDict::parent(int second) const {
    return dict_->parent(second);
}

int StatDict::freq(int index) const {
    return index < symbol_freqs_.size() ? symbol_freqs_[index] : 0;
}

double StatDict::code_length_per_char() const {
    double sum = 0;
    for (int i = 0; i < size(); i++) {
        int frequency = freq(i);
        if (frequency > 0)
            sum -= frequency * log(frequency);
    }
    return (sum + power_ * log(power_)) / total_chars_;
}

double StatDict::weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double total_freq,
                                  std::vector<int>* builder, std::unordered_set<int>* excludes) {
    auto len = seq.size();
    std::vector<double> score(len + 1, std::numeric_limits<double>::lowest());
    score[0] = 0;
    std::vector<int> symbols(len + 1);

    for (int pos = 0; pos < len; pos++) {
        std::vector<int> suffix(seq.begin() + pos, seq.end());
        int sym = search(suffix, excludes);
        do {
            auto sym_len = get(sym)->size();
            double sym_log_prob = (freqs.size() > sym ? log(freqs[sym] + 1) : 0) - log(total_freq + size());

            if (score[sym_len + pos] < score[pos] + sym_log_prob)
            {
                score[sym_len + pos] = score[pos] + sym_log_prob;
                symbols[sym_len + pos] = sym;
            }
        }
        while ((sym = parent(sym)) >= 0);
    }
    std::vector<int> solution(len + 1);
    int pos = len;
    int index = 0;
    while (pos > 0) {
        int sym = symbols[pos];
        solution[len - (++index)] = sym;
        pos -= get(sym)->size();
    }
    for (int i = 0; i < index; i++) {
        builder->push_back(solution[len - index + i]);
    }
    return score[len];
}

//TODO change types and args
StatDict StatDict::reduce(int slots) {
    std::vector<std::vector<int>> new_dict(size());
    std::vector<StatItem> items = filter_stat_items(slots);
    std::vector<int> freqs(items.size());
    double power = 0.0;
    for (const auto& item : items)
        power += item.count();

    double min_prob_result = std::min(1. / size(), kMaxMinProbability);
    for (const auto& item : items) {
        double p = (item.count() + 1.0) / (power + size());
        if (parent(item.second()) >= 0)
            min_prob_result = std::min(p, min_prob_result);
        const std::vector<int>* symbol = get(item.second());
        new_dict.push_back(*symbol);
        freqs.push_back(item.count());
    }
    //TODO no new!
    return StatDict(new_dict, min_prob_result, &freqs);
    // return nullptr;
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
    std::vector<StatItem> items;
    stat_items(&items, &excludes);
    while (items.size() > std::min((int)items.size(), slots))
        items.pop_back();
    return items;
}

int StatDict::stat_items(std::vector<StatItem>* items, std::unordered_set<int>* excludes) {
    double code_length = code_length_per_char() * total_chars_;
    for (int id = 0; id < symbol_freqs_.size(); id++) {
        if (excludes->count(id) == 0) {
            int count = freq(id);
            const std::vector<int>* seq = get(id);
            if (seq->size() > 1) {
                std::vector<int> parse;
                excludes->insert(id);
                weightedParse(*seq, symbol_freqs_, power_, &parse, excludes); //TODO doesn't work, make base_dict
                excludes->erase(id);
                double new_power = power_ + (parse.size() - 1) * count;
                double code_length_without_symbol = code_length + count * log(count) - power_ * log(power_) + new_power * log(new_power);
                for (int next : parse) {
                    int old_freq = freq(next);
                    int new_freq = old_freq + count;
                    code_length_without_symbol -= new_freq * log(new_freq) - (old_freq > 0 ? old_freq * log(old_freq) : 0);
                }
                double score = code_length_without_symbol - code_length;
                if (score > 0) {
                    items->emplace_back(StatItem(-1, id, score, count));
                }
            } else {
                items->emplace_back(StatItem(-1, id, std::numeric_limits<double>::max(), count));
            }
        }
    }
    std::sort(items->begin(), items->end(), [] (const StatItem& a, const StatItem& b) { return -a.score() < -b.score(); });
    return items->size();
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
            std::int64_t code = (((std::int64_t) first) << 32) | second;
            if (old_pairs.at(code) != new_pairs.at(code)) {
                std::cout << "\t" << dict_->get(first) << "|" << dict_->get(second) << ": " << old_pairs.at(code) <<
                          " -> " << new_pairs.at(code) << std::endl;
            }
        }
    }
}

// TODO change
StatDict StatDict::expand(int slots) {
    std::vector<StatItem> items;
    std::unordered_set<std::vector<int>, VectorHash> known;
    for (const auto& seq : *alphabet()) {
        known.insert(seq);
        items.emplace_back(StatItem(-1, search(seq), std::numeric_limits<double>::max(), freq(search(seq))));
    }
    slots += alphabet()->size();
    std::vector<double> start_with(symbol_freqs_.size());
    std::vector<double> ends_with(symbol_freqs_.size());
    for (auto &pairs_freq : pairs_freqs_) {
        std::int64_t code = pairs_freq.first;
        int freq = pairs_freq.second;
        int first = code >> 32;
        int second = code & 0xFFFFFFFFL;
        // !!! TODO check it
        start_with[first] += freq;
        ends_with[second] += freq;
    }

    double total_pair_freqs = std::accumulate(start_with.begin(), start_with.end(), 0.0);
    for (auto &pairs_freq : pairs_freqs_) {
        std::int64_t code = pairs_freq.first;
        int freq = pairs_freq.second;
        int first = code >> 32;
        int second = code & 0xFFFFFFFFL;
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
            //rng.nextDirichlet(dirichlet_params, sample); //TODO FastRandom from commons.random
            double pAB = sample[0];
            double pAY = sample[1];
            double pXB = sample[2];
            score += freq * pAB / (pAY + pAB) * log(pAB / (pAY + pAB) / (pXB + pAB)) / samples_count;
        }

        StatItem item(first, second, score, freq);
        // !!!
        std::vector<int> item_text;
        item.text(&item_text);
        if (known.count(item_text) == 0) {
            known.insert(item_text);
            items.push_back(item);
        }
    }

    std::sort(items.begin(), items.end(), [] (const StatItem& a, const StatItem& b) { return -a.score() < -b.score(); });
    std::vector<std::vector<int>> new_dict;
    std::vector<int> freqs;
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
        std::vector<int> item_text;
        item.text(&item_text);
        new_dict.push_back(item_text);
        freqs.push_back(item.count());
        if (item.first() >= 0)
            min_prob_result = std::min(min_prob_result, item.count() / accumulated_freqs); // !!! maybe accumulated_freqs is wrong
    }
    // TODO check this line, it's mistake
    return StatDict(new_dict, min_prob_result, &freqs);
    // return nullptr;
}

bool StatDict::enough(double prob_found) {
    return power_ > -log(prob_found) / min_probability_;
}

//void visitAssociations(int start, TIntDoubleProcedure procedure) {
//    pairsFreqs.visitRange(((long) start) << 32, ((long) start + 1L) << 32, (a, b) -> procedure.execute((int)(a & 0x7FFFFFFFL), b));
//}

int StatDict::parse(const std::vector<int>& seq, std::vector<int>* parse_result) {
    total_chars_ += seq.size();
    weightedParse(seq, parse_freqs_, power_ + parse_freqs_init_power_, parse_result);
    auto length = parse_result->size();
    int prev = -1;
    for (int i = 0; i < length; ++i) {
        int symbol = (*parse_result)[i];
        update_symbol(symbol, 1);
        if (prev >= 0)
            pairs_freqs_[(((std::int64_t) prev) << 32) | symbol] += 1;
        prev = symbol;
    }
    return parse_result->size();
}


//StatItem

StatItem::StatItem(int first, int second, double score, int count) {
    first_ = first;
    second_ = second;
    score_ = score;
    count_ = count;
}

//std::string StatItem::to_string() {
//    std::string result = "";
//    if (first_ >= 0)
//        result += stat_dict_.get(first_) + "|";
//    result += *stat_dict_.get(second_);
//    result += "->(";
//    result += count_;
//    result += ", " + score_;
//    result += ")";
//    return result;
//}

bool StatItem::equals(const StatItem& stat_item) {
    return first_ == stat_item.first_ && second_ == stat_item.second_;
}

void StatItem::text(std::vector<int>* output) const {
    if (first_ >= 0) {
        output->insert(output->end(), stat_dict_->get(first_)->begin(), stat_dict_->get(first_)->end());
        output->insert(output->end(), stat_dict_->get(second_)->begin(), stat_dict_->get(second_)->end());
    } else {
        output->insert(output->end(), stat_dict_->get(second_)->begin(), stat_dict_->get(second_)->end());
    }
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
