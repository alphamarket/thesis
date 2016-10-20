#ifndef QLEARNING_HPP
#define QLEARNING_HPP

#include <stdafx.hpp>

struct QLearningResult;
struct QLearningOptions;

struct QLearningResult {
    vector<size_t> hops;
    vector<QLearningOptions> opts;
    size_t iteration_max;
};

struct QLearningOptions {
    scalar
        alpha,
        gamma;
};

#endif // QLEARNING_HPP
