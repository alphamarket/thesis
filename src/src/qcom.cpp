#include "inc/qcom.hpp"
#include "rl/qlearning.hpp"
#include "qcom/qcom.fci.hpp"

qtable_t QCom::combine() const {
    return fci_execute(this);
}
