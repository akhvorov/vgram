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
        if (i >= out->size()) {
            std::cout << "Error in fast_random.cc 1" << std::endl;
        }
        (*out)[i] = gamma;
        total += gamma;
    }
    double invTotal = 1.0 / total;
    for (int i = 0; i < params.size(); ++i) {
        if (i >= out->size()) {
            std::cout << "Error in fast_random.cc 2" << std::endl;
        }
        (*out)[i] = (*out)[i] * invTotal;
    }
}
