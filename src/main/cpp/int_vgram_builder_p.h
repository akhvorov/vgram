//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_P_H
#define VGRAM_INT_VGRAM_BUILDER_P_H

#include "int_vgram_builder.h"
#include "int_dict.h"

class IntVGramBuilderImpl : public IntVGramBuilder {
public:
  const double kExtensionFactor = 1.3;
  const double kMaxPower = 20000000;
  const double kMaxMinProbability = 0.002;
  const int kAggPower = 100000;

  VGramBuilder(const std::vector<std::vectro<T>>& alphabet, int size);
  IntDict result() const;
  IntDict alpha() const;
  void accept(const std::vector<int>& seq);
  std::vector<int>* result_freqs() const;
  void printPairs(const std::ofstream& ps) const;
  void print(const std::ofstream& file_writer) const;
  double codeLength() const;

private:
  void update();
  double kl(const std::vector<int>& freqs, const std::unordered_map<long, int>& pair_freqs);
};


#endif //VGRAM_INT_VGRAM_BUILDER_P_H
