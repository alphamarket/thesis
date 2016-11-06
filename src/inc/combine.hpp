#ifndef COMBINER_HPP
#define COMBINER_HPP

#include "stdafx.hpp"

#include "fci.hpp"
#include "agent.hpp"
#include "plugins/plugin_sep.hpp"
#include "plugins/plugin_reference_counter.hpp"

#include <unordered_map>

class combine
{
public:
    template<size_t state_dim, size_t action_dim>
    combine(vector<agent<state_dim, action_dim>>& agents, const string& method, const string& second_method)
    {
        flag_workflow();
        assert(agents.size() != 0);
        if(method == "sep") this->sep_combiner(agents);
        else if(method == "refmat") this->refmat_combiner(agents, second_method);
        else if(method == "sep-refmat") {
            this->sep_combiner(agents);
            this->refmat_combiner(agents, second_method);
        }
        else raise_error("Undefined method# " + method);
    }
protected:
    /**
     * @brief The SEP combiner
     */
    template<size_t state_dim, size_t action_dim>
    void sep_combiner(vector<agent<state_dim, action_dim>>& agents, const string& = "") {
        flag_workflow();
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
                    gw[i] += (shocks[_g]->slice(ii).sum());
                }
            }
            // calculate the Qco values from agents' Qs
            for(size_t i = 0; i < 2; i++) {
                for(auto _g : g[i]) {
                    size_t k = 0;
                    auto q = Qs[_g]->slice(ii);
                    scalar w = 1 / g[i].size();
                    if(gw[i] != 0) w = (shocks[_g]->slice(ii).sum()) / gw[i];
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
        // replace combined SEPs
        for(size_t i = 0; i < agents.size(); i++) {
            auto sepp = agents[i].template get_plugin<plugin_SEP>();
            sepp->sep = combined_sep;
        }
    }
    /**
     * @brief The FCI method combiner
     */
    template<size_t state_dim, size_t action_dim>
    void refmat_combiner(vector<agent<state_dim, action_dim>>& agents, const string& method = "")  {
        flag_workflow();
        for_each(agents.begin(), agents.end(), [](const auto& a) {
            if(a.template get_plugin<plugin_reference_counter>() == nullptr)
                raise_error("The `plugin_reference_counter` not found!");
        });
        vector<matrix<size_t, state_dim>*> refs;
        vector<matrix<scalar, state_dim + action_dim>*> Qs;
        plugin_reference_counter<state_dim, action_dim>* refmat = nullptr;
        matrix<scalar, state_dim + action_dim> QCO(agents.front().learner->Q.size());
        QCO = 0;
        for(size_t i = 0; i < agents.size(); i++) {
            refmat = agents[i].template get_plugin<plugin_reference_counter>();
            refs.push_back(&refmat->refmat);
            Qs.push_back(&agents[i].learner->Q);
        }
        // combiner methods manifest
        unordered_map<string, fci_combiner_func_t> method_manifest = {
            {"wsum", nullptr},
            {"fci-max", fci::combiner_max},
            {"fci-mean", fci::combiner_mean},
            {"fci-k-mean", fci::combiner_k_mean},
        };
        if(!method_manifest.count(method))
            raise_error("undefined fci method `"+method+"`");

        // fetch the combiner methone
        auto fci_combine_method = method_manifest.at(method);

        // get all states indices
        auto li = QCO.template list_indices<state_dim>();
        // foreach state
        for(auto ii : li) {
            auto ss = refmat->ref_state(ii);
            vector<scalar> factors;
            // compute the impact factors of each agant
            {
                matrix1D_t<size_t> aref({ agents.size() });
                size_t k = 0;
                for(auto r : refs) aref[k++] = (r->operator()(ss) + 1);
                factors = ((aref.normalize() * -1) + 1).to_vector();
            }
            assert(factors.size() == agents.size());
            // Q value for every actions of current state of all agents
            vector<vector<scalar>> qa;
            for(auto q : Qs) qa.push_back(q->slice(ii).to_vector());
            size_t k = 0;
            scalar factors_sum = accumulate(factors.begin(), factors.end(), 0.0);
            // for each action in this state
            QCO.slice_ref(ii).for_each([&k, &qa, &factors, &fci_combine_method, method, factors_sum](auto* i) {
                vector<scalar> q;
                for(auto qq : qa) q.push_back(qq[k]);
                // now we have:
                //      Q value for current state/action/agent  [q]
                //      Ref value for current state/agent       [factors]
                // calculate the combined value for current state/action
                if(fci_combine_method != nullptr)
                    *i = fci::combine(q, factors, fci_combine_method);
                else if(method == "wsum") {
                    // do the weighted sum
                    for(size_t j = 0; j < q.size(); j++) *i += q[j] * factors[j] / factors_sum;
                } else
                    raise_error("undefined method `"+method+"`");
                assert(!isnan(*i));
                k += 1;
            });
        }
        size_t k = 0;
        // copy the Q table to all agents
        QCO.for_each([&k, &Qs](const auto& i){
            for(auto q : Qs)
                q->data()[k] = i;
            k += 1;
        });
        assert(k == QCO.num_elements());
    }
};

#endif // COMBINER_HPP
