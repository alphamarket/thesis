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

class maze_learner : public IQlearner<2, 1>
{
    typedef IQlearner<2, 1> base;
protected:
    inline auto get_actions_q(const array<size_t, 2>& s) const
    { return this->Q[indices[s[0]][s[1]][range(0, this->Q.size().back())]]; }

public:
    maze_learner(const array<size_t, 3>& sizes)
        : base(sizes)
    { }

    virtual scalar update(const state_action_t& sa, const array<size_t, 2>& sprim, const scalar& reward, const scalar& beta, const scalar& gamma) {
        auto q = this->get_actions_q(sprim);
        this->Q(sa) = (1 - beta) * this->Q(sa) + beta * (reward + gamma * *std::max_element(q.begin(), q.end()));
        return this->Q(sa);
    }

    virtual size_t advise_action_greedy(const base::state_t& s, const scalar& explore_rate) const {
        if(frand() < explore_rate) return get_rand(0, this->Q.size().back());
        auto q = this->get_actions_q(s);
        return std::max_element(q.begin(), q.end()) - q.begin();
    }

    virtual size_t advise_action_boltzmann(const base::state_t& s, const scalar& tau) const {
        scalar sum = 0;
        size_t index = 0;
        scalar p = frand();
        vector<pair<size_t, scalar>> eq;
        // get current action values
        auto q = this->get_actions_q(s);
        // calc the exp(Q/tau)
        foreach_elem(act, q) { eq.push_back({index++, exp(act / tau)}); sum += eq.back().second; }
        // if the sum is inf, assign the biggest value possible to be able to proceed
        sum = min(sum, numeric_limits<scalar>::max());
        // normalize them
        foreach_elem(&e, eq) e.second /= sum;
        // sort in ascending order
        sort(eq.begin(), eq.end(), [](auto i, auto j) { return i.second < j.second; });
        // find the probed index
        sum = 0; foreach_elem(e, eq) { if(p < sum + e.second) return e.first; sum += e.second; }
        // we never should reach this line!
        throw runtime_error("The PC should not reach this!");
    }
};

#endif // QLEARNER_HPP
