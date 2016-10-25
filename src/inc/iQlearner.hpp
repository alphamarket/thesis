#ifndef QLEARNER_HPP
#define QLEARNER_HPP

#include "stdafx.hpp"

template<size_t state_dim, size_t action_dim>
class IQlearner
{
public:
    typedef array<size_t, state_dim> state_t;
    typedef array<size_t, action_dim> action_t;
    typedef matrix<scalar, state_dim + action_dim> qtable_t;
    typedef array<size_t, state_dim + action_dim> state_action_t;

public:
    qtable_t Q;

public:
    IQlearner(const array<size_t, state_dim + action_dim>& sizes)
        : Q(qtable_t(sizes))
    { this->Q = 0; }

    virtual scalar update(const state_action_t& sa, const state_t& sprim, const scalar& reward, const scalar& beta, const scalar& gamma) = 0;

    virtual size_t advise_action_greedy(const state_t& s, const scalar& explore_rate) const = 0;

    virtual size_t advise_action_boltzmann(const state_t& s, const scalar& tau) const = 0;
};

#endif // QLEARNER_HPP
