//
// Created by akhvorov on 14.08.18.
//

#include <numeric>
#import "int_vgram_builder_p.h"
#import "int_dict_p.h"

constexpr double StatDict::kMaxMinProbability;

IntVGramBuilderImpl::IntVGramBuilderImpl(int size) {
    size_ = size;
    symb_alphabet_ = std::vector<IntSeq>();
    current_ = std::shared_ptr<StatDict>(new StatDict(StatDict::kMaxMinProbability)); // Sanitizer: indirect leak
    result_ = nullptr;
}

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntDict& alphabet, int size) {
    init(alphabet, size);
}

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntSeq& alphabet, int size) {
    IntDictImpl int_dict(alphabet);
    init(int_dict, size);
}

void IntVGramBuilderImpl::init(const IntDict& alphabet, int size) {
    size_ = size;
    //trace = trace;
    symb_alphabet_ = alphabet.alphabet();
    current_ = std::shared_ptr<StatDict>(new StatDict(alphabet.alphabet(), StatDict::kMaxMinProbability)); // Sanitizer: indirect leak
    result_ = nullptr;
}

IntDict* IntVGramBuilderImpl::result() const {
    return result_ != nullptr ? result_->dict_.get() : nullptr;
}

const IntDict* IntVGramBuilderImpl::alpha() const {
    return new IntDictImpl(current_->alphabet());
}

void IntVGramBuilderImpl::accept(const IntSeq& seq) {
    IntSeq result;
    current_->parse(seq, &result);
    if (current_->enough(prob_found_) || current_->power_ > kMaxPower)
        update();
}

void IntVGramBuilderImpl::update() {
    if (!((current_->enough(prob_found_) || current_->power_ > kMaxPower)))
        return;
    double sum = 0;
    double text_length = 0;
    for (int i = 0; i < current_->size(); i++) {
        int freq = current_->freq(i);
        text_length += current_->get(i).size() * freq;
        if (freq > 0)
            sum -= freq * log(freq) / log(2);
    }
    double code_length = (sum + current_->power_ * log(current_->power_) / log(2)) / 8.;
    double compression_rate = code_length / text_length;
    if (compression_rate < best_compression_rate_) {
        best_compression_rate_ = compression_rate;
        no_rate_increase_turns_ = 0;
    } else if (++no_rate_increase_turns_ > 3) {
        prob_found_ *= 0.8;
    }

    std::shared_ptr<StatDict> result;
    std::vector<IntSeq> new_dict;
    IntSeq freqs;
    int alphabet_size = static_cast<int>(symb_alphabet_.size());
    if (populate_) {
        int slots;
        if (current_->size() * kExtensionFactor < 10)
            slots = size_ - alphabet_size;
        else
            slots = static_cast<int>(current_->size() * kExtensionFactor);
        double min_prob_result = current_->expand(slots, &new_dict, &freqs);
        result = std::shared_ptr<StatDict>(new StatDict(new_dict, min_prob_result, &freqs));
    }
    else {
        double min_prob_result = current_->reduce(size_ - alphabet_size, &new_dict, &freqs);
        result = std::shared_ptr<StatDict>(new StatDict(new_dict, min_prob_result, &freqs));
        // delete result_;
        result_ = result;
    }
    //delete current_;
    current_ = result;
    int alpha_accum_num = 0;
    for (int i = 0; i < current_->size(); i++) {
        if (current_->parent(i) < 0) {
            if (alpha_accum_num > alphabet_size) {
                symb_alphabet_.push_back(current_->get(i));
            }
            alpha_accum_num++;
        }
    }
    populate_ = !populate_;
}

IntSeq* IntVGramBuilderImpl::result_freqs() {
  if (result_->size() > result_->symbol_freqs_.size())
    for (auto i = result_->symbol_freqs_.size(); i < result_->size(); i++)
      result_->symbol_freqs_.push_back(0);
  return &result_->symbol_freqs_;
}

//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;

double IntVGramBuilderImpl::code_length() const {
  return result_->code_length_per_char();
}

double IntVGramBuilderImpl::kl(const IntSeq& freqs, const std::unordered_map<std::int64_t, int>& pair_freqs) const {
    std::vector<double> freq_first(freqs.size());
    for (auto &e : pair_freqs) {
        std::int64_t code = e.first;
        int freq = e.second;
        freq_first[(int)(code >> 32)] = freq;
    }

    //TODO maybe accumulate can be removed
    double total_pair_freqs = std::accumulate(freq_first.begin(), freq_first.end(), 0.0);
    double total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0.0);

    //why array?
    double result[] = {0};
    for (auto &e : pair_freqs) {
        std::int64_t code = e.first;
        double freq = e.second;
        double first = (int) (code >> 32);
        double second = (int) (code & 0xFFFFFFFFL);
        double pAB = freq / total_pair_freqs;
        double pBcondA = freq / freq_first[first];
        double pA = freqs[first] / total_freqs;
        double pB = freqs[second] / total_freqs;
        result[0] += freq * pBcondA * log(pAB / pA / pB);
    }
    return result[0];
}