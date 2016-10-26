#ifndef QLEARNER_MAZE_HPP
#define QLEARNER_MAZE_HPP

#include "iQlearner.hpp"

class learner_maze : public IQlearner<2, 1>
{
    typedef IQlearner<2, 1> base;

public:
    learner_maze(const array<size_t, 3>& sizes)
        : base(sizes)
    { }

    virtual matrix<size_t, 2> get_policy() const {
        matrix<size_t, 2> out({this->Q.size()[0], this->Q.size()[1]});
        for(size_t i = 0; i < this->Q.size()[0]; i++) {
            for(size_t j = 0; j < this->Q.size()[1]; j++) {
                auto q = this->get_actions_q({i, j});
                out({i, j}) = size_t(std::max_element(q.begin(), q.end()) - q.begin());
            }
        }
        return out;
    }
};


#endif // QLEARNER_MAZE_HPP
