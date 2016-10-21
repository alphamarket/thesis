#ifndef QLEARNING_HPP
#define QLEARNING_HPP

#include "stdafx.hpp"
#include "inc/worlds/maze.hpp"

typedef uint action;
typedef vector<vector<action>> policy_t;
typedef vector<vector<vector<scalar>>> qtable_t;

struct QLearningResult;
struct QLearningOptions;

struct QLearningResult {
    vector<size_t> _hops;
    vector<QLearningOptions> _opts;
    size_t _iteration_max;
    qtable_t _qTable;
    policy_t _policy;

    QLearningResult() { }

    QLearningResult(
        vector<size_t> _hops,
        vector<QLearningOptions> _opts,
        size_t _iteration_max,
        qtable_t _qTable,
        policy_t _policy) {
        this->_hops = _hops;
        this->_opts = _opts;
        this->_iteration_max = _iteration_max;
        this->_qTable = _qTable;
        this->_policy = _policy;
    }
};

struct QLearningOptions {
    scalar
        alpha,
        gamma;
};

#endif // QLEARNING_HPP
