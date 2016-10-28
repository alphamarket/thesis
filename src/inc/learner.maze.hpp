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

    virtual matrix<size_t, 2> get_policy() const;
};


#endif // QLEARNER_MAZE_HPP
