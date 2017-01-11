#ifndef COMBINER_HPP
#define COMBINER_HPP

#include "stdafx.hpp"
#include "cominers/combiner_mce.hpp"
#include "cominers/combiner_sep.hpp"
#include "cominers/combiner_refmat.hpp"

class combine
    : combiner_mce, combiner_sep, combiner_refmat
{
public:
    template<size_t state_dim, size_t action_dim>
    combine(vector<agent<state_dim, action_dim>>& agents, const string& method, const string& second_method)
    {
        flag_workflow();
        if(agents.size() == 0)
            raise_error("The agents' size cannot be zero!");
        if(method == "sep")
            this->sep_combiner(agents, second_method);
        else if(method == "mce")
            this->mce_combiner(agents, second_method);
        else if(method == "refmat")
            this->refmat_combiner(agents, second_method);
        else if(method == "sep-refmat") {
            this->sep_combiner(agents, second_method);
            this->refmat_combiner(agents, second_method);
        } else
            raise_error("Undefined method# " + method);
    }
};

#endif // COMBINER_HPP
