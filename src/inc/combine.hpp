#ifndef COMBINER_HPP
#define COMBINER_HPP

#include "stdafx.hpp"

#include "agent.hpp"
#include "plugins/plugin_sep.hpp"

#include <unordered_map>

class combine
{
public:
    template<size_t state_dim, size_t action_dim>
    combine(vector<agent<state_dim, action_dim>>& agents, const string& method, const string& second_method)
    {
        assert(agents.size() != 0);
        if(method == "sep") this->sep_combiner(agents);
        else if(method == "fci") this->fci_combiner(agents, second_method);
        else raise_error("Undefined method# " + method);
    }
    template<size_t state_dim, size_t action_dim>
    void sep_combiner(vector<agent<state_dim, action_dim>>& agents, const string& = "") {
        vector<matrix<scalar, state_dim + action_dim>*> Qs;
        vector<matrix<scalar, state_dim + action_dim>*> seps;
        vector<matrix<size_t, state_dim + action_dim>*> shocks;
        matrix<scalar, state_dim + action_dim> combined_sep(agents.front().template get_plugin<plugin_SEP>()->sep.size());
        for(size_t i = 0; i < agents.size(); i++) {
            auto sepp = agents[i].template get_plugin<plugin_SEP>();
            seps.push_back(&sepp->sep);
            shocks.push_back(&sepp->shock);
            Qs.push_back(&agents[i].learner->Q);
        }
        // combine seps by apply min
        size_t e = 0;
        std::for_each(combined_sep.data(), combined_sep.data() + combined_sep.num_elements(), [&](auto& i) {
            scalar _min = numeric_limits<scalar>::infinity();
            for(size_t i = 0; i < agents.size(); i++)
                _min = min(_min, seps[i]->data()[e]);
            i = _min, e++;
        });
    }
    template<size_t state_dim, size_t action_dim>
    void fci_combiner(vector<agent<state_dim, action_dim>>&, const string& = "")  {

    }
};

#endif // COMBINER_HPP
