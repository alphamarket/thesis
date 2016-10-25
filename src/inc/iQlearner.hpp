#ifndef QLEARNER_HPP
#define QLEARNER_HPP

#include "stdafx.hpp"

template<size_t state_dim, size_t action_dim>
class IQlearner
{
protected:
    typedef state_t<state_dim> state;
    typedef state_t<action_dim> action;
    typedef matrix<scalar, state_dim + action_dim> qtable;
    typedef state_t<state_dim + action_dim> state_action;

public:
    qtable Q;

public:
    IQlearner(const array<size_t, state_dim + action_dim>& sizes)
        : Q(qtable(sizes))
    { this->Q = 0; }

    virtual scalar update(const state& s, const action& a, const state& sprim, const scalar& reward, const scalar& beta, const scalar& gamma) = 0;

    virtual action advise_action_greedy(const state& s, const scalar& explore_rate) const = 0;

    virtual action advise_action_boltzmann(const state& s, const scalar& tau) const = 0;

    virtual vector<scalar> get_actions_q(const array<size_t, state_dim>& s) const = 0;

    virtual matrix<size_t, state_dim> get_policy() const = 0;

};

#endif // QLEARNER_HPP
