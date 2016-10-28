#include "inc/learner.maze.hpp"


matrix<size_t, 2> learner_maze::get_policy() const {
    matrix<size_t, 2> out({this->Q.size()[0], this->Q.size()[1]});
    for(size_t i = 0; i < this->Q.size()[0]; i++) {
        for(size_t j = 0; j < this->Q.size()[1]; j++) {
            auto q = this->get_actions_q({i, j});
            out({i, j}) = size_t(std::max_element(q.begin(), q.end()) - q.begin());
        }
    }
    return out;
}
