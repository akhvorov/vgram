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

/*FastRandom::FastRandom() {

}

FastRandom::FastRandom(std::int64_t seed) {

}

int FastRandom::next(int bits) {
    seed_ = (seed_ * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (int)(seed_ >> (48 - bits));
}

double FastRandom::next_double() {
    return ((((std::int64_t)next(26)) << 27) + next(27)) / (double)(1L << 53);
}

double FastRandom::next_gaussian() {
    return sqrt(-2. * log(next_double())) * cos(2 * M_PI * next_double());
}

double FastRandom::next_standard_exponential() {
    return -log(1.0 - next_double());
}

double FastRandom::next_standard_gamma(const double shape) {
    double b, c;
    double U, V, X, Y;

    if (shape == 1.0) {
        return next_standard_exponential();
    } else if (shape < 1.0) {
        for (; ; ) {
            U = next_double();
            V = next_standard_exponential();

            if (U <= 1.0 - shape) {
                X = pow(U, 1. / shape);

                if (X <= V) {
                    return X;
                }
            } else {
                Y = -log((1 - U) / shape);
                X = pow(1.0 - shape + shape * Y, 1. / shape);

                if (X <= (V + Y)) {
                    return X;
                }
            }
        }
    } else {
        b = shape - 1. / 3.;
        c = 1. / sqrt(9 * b);
        for (; ; ) {
            do {
                X = next_gaussian();
                V = 1.0 + c * X;
            } while (V <= 0.0);

            V = V * V * V;
            U = next_double();
            if (U < 1.0 - 0.0331 * (X * X) * (X * X)) {
                return (b * V);
            }

            if (log(U) < 0.5 * X * X + b * (1. - V + log(V))) {
                return (b * V);
            }
        }
    }
}

void FastRandom::next_dirichlet(const std::vector<double>& params, std::vector<double>* out) {
    double total = 0;
    double gamma;
    std::default_random_engine generator;
    for (int i = 0; i < params.size(); ++i) {
        gamma = std::gamma_distribution<double>(params[i], 1)(generator);
        //gamma = next_standard_gamma(params[i]);
        (*out)[i] = gamma;
        total += gamma;
    }
    double invTotal = 1.0 / total;
    for (int i = 0; i < params.size(); ++i) {
        (*out)[i] = (*out)[i] * invTotal;
    }
}*/