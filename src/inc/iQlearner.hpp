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

    virtual matrix<size_t, state_dim> get_policy() const = 0;

    virtual inline matrix<scalar, action_dim> get_actions_q(const array<size_t, state_dim>& s) const { return this->Q.slice(slice<state_dim>(s)); }

    virtual scalar update(const state& s, const action& a, const state& sprim, const scalar& reward, const scalar& beta, const scalar& gamma) {
        auto q = this->get_actions_q(sprim);
        auto sa = this->combine_state_action(s, a);
        this->Q(sa) = (1 - beta) * this->Q(sa) + beta * (reward + gamma * *std::max_element(q.begin(), q.end()));
        return this->Q(sa);
    }

    virtual action advise_action_greedy(const state& s, const scalar& explore_rate) const {
        if(frand() < explore_rate) return {get_rand(0, this->Q.size().back())};
        auto q = this->get_actions_q(s);
        return {size_t(std::max_element(q.begin(), q.end()) - q.begin())};
    }

    virtual action advise_action_boltzmann(const state& s, const scalar& tau) const {
        // get current action values
        return {this->advise_boltzmann(this->get_actions_q(s), tau)};
    }

    template<typename T, size_t sdim>
    size_t advise_boltzmann(const matrix<T, sdim>& q, const scalar& tau) const {
        scalar sum = 0;
        size_t index = 0;
        scalar p = frand();
        vector<pair<size_t, scalar>> eq;
        // calc the exp(Q/tau)
        for(auto act : q) {
            auto pp = make_pair(index++, exp(act / tau));
            eq.push_back(pp);
            sum += eq.back().second;
        }
        // if the sum is inf, assign the biggest value possible to be able to proceed
        sum = min(sum, numeric_limits<scalar>::max());
        // normalize them
        for(auto &e : eq) e.second /= sum;
        // sort in ascending order
        sort(eq.begin(), eq.end(), [](auto i, auto j) { return i.second < j.second; });
        // find the probed index
        sum = 0; for(auto e : eq) { if(p < sum + e.second) return e.first; sum += e.second; }
        // we never should reach this line!
        throw runtime_error("The PC should not reach this!");
    }

    template<size_t ss, size_t aa>
    inline array<size_t, ss + aa> combine_state_action(const array<size_t, ss>& s, const array<size_t, aa>& a) const {
        array<size_t, ss + aa> sa;
        copy_n(s.begin(), ss, sa.begin());
        copy_n(a.begin(), aa, sa.begin() + ss);
        return sa;
    }

};

#endif // QLEARNER_HPP
