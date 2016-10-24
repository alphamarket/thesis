#include "inc/qcom.hpp"
#include "rl/qlearning.hpp"
#include "qcom/qcom.fci.hpp"
#include "qcom/qcom.sep.hpp"

#include <unordered_map>

qtable_t QCom::combine(const string &method) const {
    unordered_map<string, function<qtable_t(const QCom* const)>> methods = {
        {"fci", fci_execute},
        {"sep", sep_execute}
    };
    return methods[method](this);
}
