#ifndef WORLD_MAZE_HPP
#define WORLD_MAZE_HPP

#include "iworld.hpp"

class maze : public IWorld<2>
{
    const size_t ACTIONS_NO = 4;

    matrix2D_t<block> _maze;

public:
    enum moves { UP = 0, RIGHT, DOWN, LEFT };

    maze(const array<size_t, 2>& sizes)
        : base(sizes, ACTIONS_NO), _maze(matrix2D_t<block>({sizes[0], sizes[1]}))
    {
        this->_maze = block(0, 0, 1);
        this->set_current_state({0, 0});
    }

    inline matrix2D_t<block>& get_maze() { return this->_maze; }
    inline matrix2D_t<block> get_maze() const { return this->_maze; }

    inline bool is_terminated() const { return this->get_current_block()._is_terminal; }

    inline block& get_current_block() { return this->_maze[this->_state[0]][this->_state[0]]; }
    inline block get_current_block() const { return this->_maze[this->_state[0]][this->_state[0]]; }

    bool make_move(const size_t& action_id) {
        switch(action_id) {
        case UP:
            if(this->get_current_state().y() > 0) {
                this->get_current_state().y()--;
                return true;
            }
            return false;
        case RIGHT:
            if(this->get_current_state().x() < this->size()[1] - 1) {
                this->get_current_state().x()++;
                return true;
            }
            return false;
        case DOWN:
            if(this->get_current_state().y() < this->size()[0] - 1) {
                this->get_current_state().y()++;
                return true;
            }
            return false;
        case LEFT:
            if(this->get_current_state().x() > 0) {
                this->get_current_state().x()--;
                return true;
            }
            return false;
        default:
            error("Undefined move# " + to_string(action_id));
        }
    }

    virtual void define_values(const vector<pair<state, block>>& values) {
        foreach_elem(&psb, values)
            this->_maze[psb.first[0]][psb.first[1]] = psb.second;
    }

};

#endif // WORLD_MAZE_HPP
