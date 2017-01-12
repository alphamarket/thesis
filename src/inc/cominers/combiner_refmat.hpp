#ifndef COMBINER_REFMAT_HPP
#define COMBINER_REFMAT_HPP

#include "inc/stdafx.hpp"

#include "inc/fci.hpp"
#include "inc/agent.hpp"
#include "inc/plugins/plugin_reference_counter.hpp"

#include <unordered_map>

class combiner_refmat {
protected:
    /**
     * @brief The refmat method combiner
     */
    template<size_t state_dim, size_t action_dim>
    void refmat_combiner(vector<agent<state_dim, action_dim>>& agents, const string& method = "") {
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
            {"fci-const-one", fci::combiner_const_one},
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
                size_t k = 0;
                matrix1D_t<size_t> aref({ agents.size() });
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

#endif // COMBINER_REFMAT_HPP
