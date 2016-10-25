#ifndef WORLD_HPP
#define WORLD_HPP

#include "stdafx.hpp"

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

template<size_t state_dim, size_t action_dim>
class IWorld
{
protected:
    typedef IWorld<state_dim, action_dim> base;

    const size_t _actions_no;
    state_t<state_dim> _state;
    matrix<block, state_dim> _maze;
    const array<size_t, state_dim> _size;

public:
    const static size_t STATE_DIM = state_dim;
    const static size_t ACTION_DIM = action_dim;

public:

    IWorld(const array<size_t, state_dim>& size, const size_t& actions_no)
        : _actions_no(actions_no), _maze(matrix<block, state_dim>(size)), _size(size)
    { this->_maze = block(0, 0, 1); }

    virtual ~IWorld() { }

    virtual inline size_t action_no() const { return this->_actions_no; }
    virtual inline array<size_t, state_dim> size() const { return this->_size; }

    virtual inline state_t<state_dim>& get_current_state() { return this->_state; }
    virtual inline state_t<state_dim> get_current_state() const { return this->_state; }
    virtual inline void set_current_state(const state_t<state_dim>& s) { this->_state = s; }

    virtual inline matrix<block, state_dim>& get_maze() { return this->_maze; }
    virtual inline matrix<block, state_dim> get_maze() const { return this->_maze; }

    virtual inline bool is_terminated() const { return this->get_current_block()._is_terminal; }

    virtual inline block& get_current_block() { return this->_maze[this->_state[0]][this->_state[1]]; }
    virtual inline block get_current_block() const { return this->_maze[this->_state[0]][this->_state[1]]; }

    virtual inline void define_values(const vector<pair<state_t<state_dim>, block>>& values)
    { foreach_elem(&psb, values) this->_maze[psb.first[0]][psb.first[1]] = psb.second; }

public:

    virtual state_t<state_dim> get_random_state() const = 0;

    virtual scalar make_move(const state_t<action_dim>& action_id) = 0;

    virtual string move_tostring(const state_t<action_dim>& move) const = 0;
};

#endif // WORLD_HPP
