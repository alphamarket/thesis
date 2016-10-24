#ifndef QLEARNING_HPP
#define QLEARNING_HPP

#include "stdafx.hpp"
#include "inc/sep.hpp"
#include "inc/worlds/maze.hpp"

typedef uint action;
typedef vector<vector<action>> policy_t;
typedef vector<vector<scalar>> refmat_t;
typedef vector<vector<vector<scalar>>> qtable_t;

struct QLearningResult;
struct QLearningOptions;

struct QLearningResult {
    vector<size_t> _hops;
    vector<QLearningOptions> _opts;
    size_t _iteration_max;
    qtable_t _qtable;
    policy_t _policy;
    vector<refmat_t> _refmat;
    SEP::sepmat_t _sepmat;
    SEP::shockmat_t _shockmat;

    QLearningResult() { }

    QLearningResult(
        vector<size_t> _hops,
        vector<QLearningOptions> _opts,
        size_t _iteration_max,
        qtable_t _qTable,
        policy_t _policy,
        vector<refmat_t> _refmat,
        SEP::sepmat_t _sepmat,
        SEP::shockmat_t _shockmat) {
        this->_hops = _hops;
        this->_opts = _opts;
        this->_iteration_max = _iteration_max;
        this->_qtable = _qTable;
        this->_policy = _policy;
        this->_refmat = _refmat;
        this->_sepmat = _sepmat;
        this->_shockmat = _shockmat;
    }
};

struct QLearningOptions {
    scalar
        beta,
        gamma;
};

#endif // QLEARNING_HPP
