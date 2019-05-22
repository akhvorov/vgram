//
// Created by Aleksandr Khvorov on 02.09.18.
//

#include <cmath>
#include <random>
#include <iostream>
#include "fast_random.h"

void next_dirichlet(const std::vector<double>& params, std::vector<double>* out) {
    double total = 0;
    double gamma;
    std::default_random_engine generator;
    for (int i = 0; i < params.size(); ++i) {
        gamma = std::gamma_distribution<double>(params[i], 1)(generator);
        (*out)[i] = gamma;
        total += gamma;
    }
    double invTotal = 1.0 / total;
    for (int i = 0; i < params.size(); ++i) {
        (*out)[i] = (*out)[i] * invTotal;
    }
}
