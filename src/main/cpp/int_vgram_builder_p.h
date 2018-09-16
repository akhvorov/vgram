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
  const double kExtensionFactor = 1.3;
  const double kMaxPower = 20000000;
  const double kMaxMinProbability = 0.002;
  // const int kAggPower = 100000;

  IntVGramBuilderImpl(const IntDict& alphabet, int size);
  IntVGramBuilderImpl(const IntSeq& alphabet, int size);
  IntDict* result() const;
  const IntDict* alpha() const;
  void accept(const IntSeq& seq);
  IntSeq* result_freqs();
//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;
  double codeLength() const;

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
