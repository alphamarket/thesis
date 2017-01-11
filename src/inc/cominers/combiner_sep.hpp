#ifndef COMBINER_SEP_HPP
#define COMBINER_SEP_HPP

#include "inc/stdafx.hpp"

#include "inc/fci.hpp"
#include "inc/agent.hpp"
#include "inc/plugins/plugin_sep.hpp"

#include <unordered_map>

class combiner_sep {
protected:
    /**
     * @brief The SEP-FCI combiner
     */
    template<size_t state_dim, size_t action_dim>
    void sep_combiner(vector<agent<state_dim, action_dim>>& agents, const string& method = "") {
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
        // combiner methods manifest
        unordered_map<string, fci_combiner_func_t> method_manifest = {
            {"wsum", nullptr},
            {"fci-max", fci::combiner_max},
            {"fci-mean", fci::combiner_mean},
            {"fci-k-mean", fci::combiner_k_mean},
            {"fci-const-one", fci::combiner_const_one},
        };
        if(!method_manifest.count(method))
            raise_error("undefined fci method `"+method+"`");

        // fetch the combiner methone
        auto fci_combine_method = method_manifest.at(method);
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
                if(method == "wsum") {
                    for(auto _g : g[i]) {
                        size_t k = 0;
                        auto q = Qs[_g]->slice(ii);
                        scalar w = 1 / g[i].size();
                        if(gw[i] != 0) w = (shocks[_g]->slice(ii).sum()) / gw[i];
                        QCO[i].slice_ref(ii).for_each([&w, &q, &k](auto* i) { *i += w * q.data()[k++]; });
                    }
                } else {
                    size_t k = 0;
                    vector<scalar> factors;
                    vector<matrix<scalar, action_dim>> qs;
                    for(auto _g : g[i]) {
                        factors.push_back(1. / g[i].size());
                        qs.push_back(Qs[_g]->slice(ii));
                        if(gw[i] != 0)
                            factors.back() = scalar(shocks[_g]->slice(ii).sum()) / gw[i];
                    }
                    QCO[i].slice_ref(ii).for_each([&factors, &qs, &k, &fci_combine_method](auto* i) {
                        vector<scalar> q;
                        for(auto& _q : qs) q.push_back(_q.data()[k]);
                        *i += fci::combine(q, factors, fci_combine_method);
                        k += 1;
                    });
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
};

#endif // COMBINER_SEP_HPP
