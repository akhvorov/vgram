//
// Created by Aleksandr Khvorov on 02.09.18.
//

#ifndef DICT_EXPANSION_FAST_RANDOM_H
#define DICT_EXPANSION_FAST_RANDOM_H

#include <vector>

void next_dirichlet(const std::vector<double>& params, std::vector<double>* out);

/*class FastRandom {
public:
  FastRandom();
  FastRandom(std::int64_t seed);

  int next(int bits);
  double next_double();
  double next_gaussian();
  double next_standard_exponential();
  double next_standard_gamma(double shape);
  void next_dirichlet(const std::vector<double>& params, std::vector<double>* out);

private:
  std::int64_t seed_;
};*/

#endif //DICT_EXPANSION_FAST_RANDOM_H
