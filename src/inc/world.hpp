#ifndef WORLD_HPP
#define WORLD_HPP

#include "stdafx.hpp"
#include "state.hpp"

struct block {
    scalar _value = 0;
    bool _is_moveable = 0;
    bool _is_terminal = 0;
    block() { }
    block(scalar value, bool is_terminal, bool is_moveable) {
        this->_value = value;
        this->_is_terminal = is_terminal;
        this->_is_moveable = is_moveable;
    }
};

template<size_t dim>
class world
{
protected:
    typedef world<dim> base;

    state _state;
    const size_t _actions_no;
    const array<size_t, dim> _size;

public:

    world(const array<size_t, dim>& size, const size_t& actions_no)
        : _actions_no(actions_no), _size(size)
    { }

    virtual ~world() { }

    inline array<size_t, dim> size() const { return this->_size; }

    virtual state& get_current_state() { return this->_state; }
    virtual state get_current_state() const { return this->_state; }
    virtual void set_current_state(const state& s) { this->_state = s; }

public:
    virtual bool is_terminated() const = 0;

    virtual block& get_current_block() = 0;
    virtual block get_current_block() const = 0;

    virtual bool make_move(const size_t& action_id) = 0;

    virtual void define_values(const vector<pair<state, block>>& values) = 0;
};

#endif // WORLD_HPP
