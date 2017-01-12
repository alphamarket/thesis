#ifndef COMBINER_MCE_HPP
#define COMBINER_MCE_HPP

#include "inc/stdafx.hpp"

#include "inc/fci.hpp"
#include "inc/agent.hpp"
#include "inc/plugins/plugin_mce.hpp"

#include <unordered_map>

class combiner_mce {
protected:
    /**
     * @brief The MCE-FCI combiner
     */
    template<size_t state_dim, size_t action_dim>
    void mce_combiner(vector<agent<state_dim, action_dim>>& agents, const string& method = "") {
        flag_workflow();
        typedef matrix<scalar, state_dim + action_dim> qtable_t;

        qtable_t CoQ;
        vector<vector<scalar>> Ws;
        vector<qtable_t*> Qs;
        vector<qtable_t > subQs;
        for(const auto& a : agents) {
            Ws.push_back(a.template get_plugin<plugin_MCE>()->get_criterias());
            Qs.push_back(&a.learner->Q);
        }
        // re-allocate the CoQ matrix with the size
        CoQ.resize(agents.back().learner->Q.size());

        // get all states indices
        auto li = CoQ.template list_indices<state_dim>();

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

        // foreach criteria
        for(size_t criteria = 0; criteria < Ws.front().size(); criteria++) {
            // create the sub-Q matrices
            subQs.push_back(qtable_t(Qs.front()->size()));
            subQs.back() = 0;

            vector<scalar> factors;
            // compute the impact factors of each agant
            {
                size_t k = 0;
                matrix1D_t<scalar> aref({ agents.size() });
                for(auto w : Ws) aref[k++] = (w[criteria]);
                aref -= aref.min() - 1;
                aref /= aref.sum();
                factors = aref.to_vector();
            }
            for(auto ii : CoQ.template list_indices<state_dim>()) {
                size_t k = 0;
                vector<vector<scalar>> qa;
                for(auto q : Qs) qa.push_back(q->slice(ii).to_vector());
                subQs.back().slice_ref(ii).for_each([&](auto* i){
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
                        for(size_t j = 0; j < q.size(); j++) *i += q[j] * factors[j];
                    } else
                        raise_error("undefined method `"+method+"`");
                    assert(!isnan(*i));
                    k += 1;
                });
            }
        }
        // sum all sub-Qs as the new CoQ
        for(const auto& subq : subQs) CoQ += subq;
        // use the new CoQ as agents' CoQ
        for(const auto& a : agents) a.template get_plugin<plugin_MCE>()->_CoQ = CoQ;
    }
};

#endif // COMBINER_MCE_HPP
