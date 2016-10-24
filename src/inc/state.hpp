#ifndef STATE_H
#define STATE_H

#include "matrix.hpp"

class state : public matrix1D_t<size_t>
{
public:
    state()
        : matrix1D_t<size_t>({2})
    { base::operator =({0, 0}); }

    state(const std::initializer_list<size_t>& l)
        : state()
    { base::operator =(l); }

    inline size_t& x() { return this->operator [](0); }
    inline size_t& y() { return this->operator [](1); }
    inline size_t x() const { return this->operator [](0); }
    inline size_t y() const { return this->operator [](1); }
};

#endif // STATE_H
