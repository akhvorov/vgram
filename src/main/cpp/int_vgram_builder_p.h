//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_P_H
#define VGRAM_INT_VGRAM_BUILDER_P_H

#include "int_vgram_builder.h"
#include "int_dict.h"
#include <fstream>
#include <unordered_map>

class DictionaryWithStat; //TODO make it

class IntVGramBuilderImpl : public IntVGramBuilder {
public:
  const double kExtensionFactor = 1.3;
  const double kMaxPower = 20000000;
  const double kMaxMinProbability = 0.002;
  const int kAggPower = 100000;

  IntVGramBuilderImpl(const std::vector<std::vector<int>>& alphabet, int size);
  IntDict* result() const;
  IntDict* alpha() const;
  void accept(const std::vector<int>& seq);
  std::vector<int>* result_freqs() const;
//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;
  double codeLength() const;

private:
  bool is_dynamic_;
  int size_;
  // ofstream trace_;
  IntDict* initial_;
  DictionaryWithStat* current_;
  bool populate_ = true;
  DictionaryWithStat* result_;

  int alphabet_size_;
  double prob_found_ = 0.1;
  double best_compression_rate_ = 1.0;
  int no_rate_increase_turns_ = 0;

  void update();
  double kl(const std::vector<int>& freqs, const std::unordered_map<long, int>& pair_freqs);
};


#endif //VGRAM_INT_VGRAM_BUILDER_P_H
