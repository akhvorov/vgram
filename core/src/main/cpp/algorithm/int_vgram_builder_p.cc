//
// Created by akhvorov on 14.08.18.
//

#include <numeric>
#include <iostream>
#import "int_vgram_builder_p.h"
#import "int_dict_p.h"

IntVGramBuilderImpl::IntVGramBuilderImpl(int size, int verbose) {
    size_ = size;
    symb_alphabet_ = std::vector<IntSeq>();
    current_ = std::make_shared<StatDictImpl>();
    result_ = nullptr;
    verbose_ = verbose;
}

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntDict &alphabet, int size, int verbose) {
    init(alphabet, size, verbose);
}

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntSeq &alphabet, int size, int verbose) {
    IntDictImpl int_dict(alphabet);
    init(int_dict, size, verbose);
}

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntDict &dict, const IntSeq &freqs,
                                         const std::vector<IntSeq> &alphabet, double min_probability, int verbose) {
    verbose_ = verbose;
    size_ = dict.size();
    IntSeq freqs_copy(freqs.begin(), freqs.end());
    std::shared_ptr<StatDictImpl> result = std::make_shared<StatDictImpl>(dict.alphabet(), min_probability,
                                                                          &freqs_copy);
    symb_alphabet_ = alphabet;
    current_ = result;
    result_ = result;
}

void IntVGramBuilderImpl::init(const IntDict &alphabet, int size, int verbose) {
    verbose_ = verbose;
    size_ = size;
    symb_alphabet_ = alphabet.alphabet();
    current_ = std::make_shared<StatDictImpl>(alphabet.alphabet());
    result_ = nullptr;
}

std::shared_ptr<IntDict> IntVGramBuilderImpl::result() const {
    return result_ != nullptr ? result_->dict_ : current_->dict_;
}

const std::shared_ptr<IntDict> IntVGramBuilderImpl::alpha() const {
    if (result_ == nullptr) {
        return std::make_shared<IntDictImpl>(current_->alphabet());
    }
    return std::make_shared<IntDictImpl>(result_->alphabet());
}

void IntVGramBuilderImpl::accept(const IntSeq &seq) {
    IntSeq result;
    current_->parse(seq, &result);
    if (current_->enough(prob_found_) || current_->power_ > kMaxPower) {
        update();
    }
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

    std::shared_ptr<StatDictImpl> result;
    std::vector<IntSeq> new_dict;
    IntSeq freqs;
    int alphabet_size = static_cast<int>(symb_alphabet_.size());
    if (populate_) {
        if (verbose_ > 0) {
            std::cout << "Size: " << current_->size() << " rate: " << compression_rate <<
                      " minimal probability: " << current_->min_probability_ << std::endl;
        }
        int slots;
        if (current_->size() * kExtensionFactor < 10)
            slots = size_ - alphabet_size;
        else
            slots = static_cast<int>(current_->size() * kExtensionFactor);
        double min_prob_result = current_->expand(slots, &new_dict, &freqs);
        result = std::make_shared<StatDictImpl>(new_dict, min_prob_result, &freqs);
    } else {
        double min_prob_result = current_->reduce(size_ - alphabet_size, &new_dict, &freqs);
        result = std::make_shared<StatDictImpl>(new_dict, min_prob_result, &freqs);
        result_ = result;
    }
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

int IntVGramBuilderImpl::result_freqs(std::shared_ptr<StatDictImpl> stat_dict, IntSeq *freqs) {
    if (stat_dict->size() > stat_dict->symbol_freqs_.size()) {
        for (auto i = stat_dict->symbol_freqs_.size(); i < stat_dict->size(); ++i) {
            stat_dict->symbol_freqs_.push_back(0);
        }
    }
    for (int symbol_freq : stat_dict->symbol_freqs_) {
        freqs->push_back(symbol_freq);
    }
    return static_cast<int>(freqs->size());
}

int IntVGramBuilderImpl::result_freqs(IntSeq *freqs) {
    if (result_ == nullptr) {
        return result_freqs(current_, freqs);
    }
    return result_freqs(result_, freqs);
}

double IntVGramBuilderImpl::code_length() const {
    return result_->code_length_per_char();
}

double IntVGramBuilderImpl::get_min_probability() const {
    return result_ != nullptr ? result_->min_probability_ : -1.0;
}
