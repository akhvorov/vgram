//
// Created by akhvorov on 14.08.18.
//

#import "int_vgram_builder_p.h"

IntVGramBuilderImpl::IntVGramBuilderImpl(const IntDict& alphabet, int size) {
    size_ = size;
    //trace = trace;
    alphabet_size_ = alphabet.size();
    initial_ = &alphabet;
    current_ = new StatDict(new IntDictImpl(alphabet.alphabet()), nullptr, kMaxMinProbability);
}

IntDict* IntVGramBuilderImpl::result() const {
    //TODO check it after dict with stats
    return result_ != nullptr ? result_.dict : nullptr;
}

IntDict* IntVGramBuilderImpl::alpha() const {
    return initial_;
}

void IntVGramBuilderImpl::accept(const std::vector<int>& seq) {
    current_.parse(seq);
    if (current_.enough(prob_found_) || current_.power > kMaxPower)
        update();
}

void IntVGramBuilderImpl::update() {
    if (!((current_.enough(probFound) || current_.power > kMaxPower)))
        return;
    double sum = 0;
    double text_length = 0;
    for (int i = 0; i < current_.size(); i++) {
        int freq = current_.freq(i);
        text_length += current_.get(i).size() * freq;
        if (freq > 0)
            sum -= freq * log(freq) / log(2);
    }
    double code_length = (sum + current_.power * log(current_.power) / log(2)) / 8.;
    double compression_rate = code_length / text_length;
    if (compression_rate < best_compression_rate_) {
        best_compression_rate_ = compression_rate;
        no_rate_increase_turns_ = 0;
    } else if (++no_rate_increase_turns_ > 3) {
        prob_found_ *= 0.8;
    }

    StatDict* result;
    if (populate) {
        result = current_;
//        if (trace != null)
//            trace << "Size: " + current.size() + " rate: " + compressionRate + " minimal probability: " + current.minProbability << std::endl;
        int slots;
        if (current_.size() * kExtensionFactor < 10)
            slots = size - alphabet_size_;
        else
            slots = (std::int32_t)(current_.size() * kExtensionFactor);
        result = current_.expand(slots, is_dynamic_);
    }
    else {
        //TODO rewrite it
        this.result = result = current_.reduce(size - alphabet_size_);
    }
    current_ = result;
    populate_ = !populate_;
}

std::vector<int>* IntVGramBuilderImpl::result_freqs() {
  //TODO rewrite for private field in dict with stats
  if (result_.size() > result_.symbol_freqs.size())
    for (int i = result_.symbol_freqs.size(); i < result_.size(); i++)
      result_.symbol_freqs.push_back(0);
  return result_.symbol_freqs;
}

//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;

double IntVGramBuilderImpl::codeLength() const {
  return result_.code_length_per_char();
}

double IntVGramBuilderImpl::kl(const std::vector<int>& freqs, const std::unordered_map<long long, int>& pair_freqs) const {
    std::vector<double> freq_first(freqs.size());
    for (auto &e : pair_freqs) {
        long long code = e.first;
        int freq = e.second;
        freq_first[(int)(code >> 32)] = freq;
    }

    double total_pair_freqs = std::accumulate(freq_first.begin(), freq_first.end(), 0);
    double total_freqs = std::accumulate(freqs.begin(), freqs.end(), 0);

    //why array?
    double result[] = {0};
    for (auto &e : pair_freqs) {
        long long code = e.first;
        double freq = e.second;
        double first = (int) (code >>> 32);
        double second = (int) (code & 0xFFFFFFFFL);
        double pAB = freq / total_pair_freqs;
        double pBcondA = freq / freq_first[first];
        double pA = freqs[first] / total_freqs;
        double pB = freqs[second] / total_freqs;
        result[0] += freq * pBcondA * log(pAB / pA / pB);
    }
    return result[0];
}