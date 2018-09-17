//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_P_H
#define VGRAM_INT_VGRAM_BUILDER_P_H

#include <fstream>
#include <unordered_map>
#include "int_vgram_builder.h"
#include "stat_dict.h"

class StatDict;

class IntVGramBuilderImpl : public IntVGramBuilder {
public:

  IntVGramBuilderImpl(const IntDict& alphabet, int size);
  IntVGramBuilderImpl(const IntSeq& alphabet, int size);
  IntDict* result() const override;
  const IntDict* alpha() const override;
  void accept(const IntSeq& seq) override;
  IntSeq* result_freqs() override;
//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;
  double codeLength() const override;

private:
  int size_;
  // ofstream trace_;
  const IntDict* initial_;
  StatDict* current_;
  bool populate_ = true;
  StatDict* result_;

  int alphabet_size_;
  double prob_found_ = 0.1;
  double best_compression_rate_ = 1.0;
  int no_rate_increase_turns_ = 0;

  void init(const IntDict& alphabet, int size);
  void update();
  double kl(const IntSeq& freqs, const std::unordered_map<std::int64_t, int>& pair_freqs) const;
};


#endif //VGRAM_INT_VGRAM_BUILDER_P_H
