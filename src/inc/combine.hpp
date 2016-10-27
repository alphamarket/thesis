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
        for_each(agents.begin(), agents.end(), [](const auto& a) {
            if(a.template get_plugin<plugin_SEP>() == nullptr)
                raise_error("The `plugin_SEP` not found!");
        });
        matrix<scalar, state_dim + action_dim> QCO[2];
        vector<matrix<scalar, state_dim + action_dim>*> Qs;
        vector<matrix<scalar, state_dim + action_dim>*> seps;
        vector<matrix<size_t, state_dim + action_dim>*> shocks;
        matrix<scalar, state_dim + action_dim> combined_sep(agents.front().template get_plugin<plugin_SEP>()->sep.size());
        for(size_t i = 0; i < 2; i++) {
            QCO[i].resize(agents.front().learner->Q.size());
            QCO[i] = 0;
        }
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
        // get all states indices
        auto li = combined_sep.template list_indices<state_dim>();
        // for each indices
        for(auto ii : li) {
            assert(ii.size() == state_dim);
            vector<size_t> g[2];
            // foreach agent get what sep/q suggest and put them into groups
            for(size_t i = 0; i < agents.size(); i++) {
                auto q = Qs[i]->slice(ii);
                auto s = seps[i]->slice(ii);
                if((min_element(s.begin(), s.end()) - s.begin()) == (max_element(q.begin(), q.end()) - q.begin()))
                    g[1].push_back(i);
                else
                    g[0].push_back(i);
            }
            // calculate the Ws fraction part
            scalar gw[2] = {0, 0};
            for(size_t i = 0; i < 2; i++) {
                for(auto _g : g[i]) {
                    gw[i] += (shocks[_g]->slice(ii).sum() + 1);
                }
            }
            // calculate the Qco values from agents' Qs
            for(size_t i = 0; i < 2; i++) {
                for(auto _g : g[i]) {
                    size_t k = 0;
                    auto q = Qs[_g]->slice(ii);
                    scalar w = (shocks[_g]->slice(ii).sum() + 1) / gw[i];
                    QCO[i].slice_ref(ii).for_each([&w, &q, &k](auto* i) { *i += w * q.data()[k++]; });
                }
            }
            // assign the computed Qco to the agents
            for(size_t i = 0; i < 2; i++) {
                for(auto _g : g[i]) {
                    size_t k = 0;
                    auto q = QCO[i].slice(ii);
                    agents[_g].learner->Q.slice_ref(ii).for_each([&q, &k](auto* const i) { *i = q.data()[k++]; });
                }
            }
        }
    }
    template<size_t state_dim, size_t action_dim>
    void fci_combiner(vector<agent<state_dim, action_dim>>&, const string& = "")  {

    }
};

#endif // COMBINER_HPP
