//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_P_H
#define VGRAM_INT_VGRAM_BUILDER_P_H

#include <cmath>
#include <fstream>
#include <unordered_map>
#include "int_vgram_builder.h"
#include "stat_dict_p.h"

class IntVGramBuilderImpl : public IntVGramBuilder {
public:

  explicit IntVGramBuilderImpl(int size, int verbose = 1);
  IntVGramBuilderImpl(const IntDict& alphabet, int size, int verbose = 1);
  IntVGramBuilderImpl(const IntSeq& alphabet, int size, int verbose = 1);
  IntVGramBuilderImpl(const IntDict& dict, const IntSeq& freqs, const std::vector<IntSeq>& alphabet,
          double min_probability, int verbose = 1);
  std::shared_ptr<IntDict> result() const override;
  const std::shared_ptr<IntDict> alpha() const override;
  void accept(const IntSeq& seq) override;
  int result_freqs(IntSeq* freqs) override;
  double code_length() const override;
  double get_min_probability() const override;

private:
  int size_;
  std::vector<IntSeq> symb_alphabet_;
  std::shared_ptr<StatDictImpl> current_;
  bool populate_ = true;
  std::shared_ptr<StatDictImpl> result_;

  double prob_found_ = 0.1;
  double best_compression_rate_ = 1.0;
  int no_rate_increase_turns_ = 0;
  int verbose_ = 0;

  void init(const IntDict& alphabet, int size, int verbose);
  void update();
//  double get_min_();
  double kl(const IntSeq& freqs, const std::unordered_map<std::int64_t, int>& pair_freqs) const;
};


#endif //VGRAM_INT_VGRAM_BUILDER_P_H
