#ifndef LEARNER_HUNTER_H
#define LEARNER_HUNTER_H

#include "iQlearner.hpp"

class learner_hunter : public IQlearner<1, 1>
{
    typedef IQlearner<1, 1> base;

public:
    learner_hunter(const array<size_t, 2>& sizes)
        : base(sizes)
    { }

    virtual matrix<size_t, 1> get_policy() const;
};


#endif // LEARNER_HUNTER_H
