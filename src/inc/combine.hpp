#ifndef COMBINER_HPP
#define COMBINER_HPP

#include "stdafx.hpp"

#include "agent.hpp"
#include "agent.plugins.hpp"

#include <unordered_map>

template<size_t state_dim, size_t action_dim>
class combine
{
    typedef agent<state_dim, action_dim> agent_t;
public:
    combine(vector<agent_t>& agents, const string& method, const string& second_method)
    {
        assert(agents.size() != 0);
        if(method == "sep") this->sep_combiner(agents);
        else if(method == "fci") this->fci_combiner(agents, second_method);
        else raise_error("Undefined method# " + method);
    }
    void sep_combiner(vector<agent_t>& agents, const string& = "") {
        matrix<scalar, state_dim + action_dim> combined_sep(agents.front().learner->Q.size());

        vector<matrix<scalar, state_dim + action_dim>*> Qs;
        vector<matrix<scalar, state_dim + action_dim>*> seps;
        vector<matrix<size_t, state_dim + action_dim>*> shocks;
        for(size_t i = 0; i < agents.size(); i++) {
            auto sepp = (plugin_SEP<state_dim, action_dim>*)agents[i].get_plugins()[plugin_SEP<state_dim, action_dim>().name()];
            seps.push_back(sepp->sep);
            shocks.push_back(sepp->shock);
            Qs.push_back(&agents[i].learner->Q);
        }
        // combine seps by apply min
        size_t e = 0;
        std::for_each(combined_sep.data(), combined_sep.data() + combined_sep.num_elements(), [&](auto& i) {
            scalar _min = numeric_limits<scalar>::max();
            for(size_t i = 0; i < agents.size(); i++)
                if(seps[i]->data()[e] < _min)
                    _min = seps[i]->data()[e];
            i = _min;
            e++;
        });

    }
    void fci_combiner(vector<agent_t>&, const string&) {

    }
};

#endif // COMBINER_HPP
