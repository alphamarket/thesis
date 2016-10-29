#include "inc/learner.hunter.hpp"

matrix<size_t, 1> learner_hunter::get_policy() const {
    matrix<size_t, 1> out({this->Q.size()[0]});
    for(size_t i = 0; i < this->Q.size()[0]; i++) {
        auto q = this->get_actions_q({i});
        out({i}) = size_t(std::max_element(q.begin(), q.end()) - q.begin());
    }
    return out;
}
