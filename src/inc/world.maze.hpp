#ifndef WORLD_MAZE_HPP
#define WORLD_MAZE_HPP

#include "iWorld.hpp"

class maze : public IWorld<2, 1>
{
    const size_t ACTIONS_NO = 4;

public:
    enum moves { UP = 0, RIGHT, DOWN, LEFT };

    maze(const array<size_t, 2>& sizes)
        : base(sizes, 4)
    { this->set_current_state({0, 0}); }

    scalar make_move(const state_t<1>& action_id) {
        switch(action_id[0]) {
        case UP:
            if(this->get_current_state()[0] > 0) {
                this->get_current_state()[0]--;
                scalar value = this->get_current_block()._value;
                if(!this->get_current_block()._is_moveable)
                    this->get_current_state()[0]++;
                return value;
            }
            return this->get_current_block()._value;
        case RIGHT:
            if(this->get_current_state()[1] < this->size()[1] - 1) {
                this->get_current_state()[1]++;
                scalar value = this->get_current_block()._value;
                if(!this->get_current_block()._is_moveable)
                    this->get_current_state()[1]--;
                return value;
            }
            return this->get_current_block()._value;
        case DOWN:
            if(this->get_current_state()[0] < this->size()[0] - 1) {
                this->get_current_state()[0]++;
                scalar value = this->get_current_block()._value;
                if(!this->get_current_block()._is_moveable)
                    this->get_current_state()[0]--;
                return value;
            }
            return this->get_current_block()._value;
        case LEFT:
            if(this->get_current_state()[1] > 0) {
                this->get_current_state()[1]--;
                scalar value = this->get_current_block()._value;
                if(!this->get_current_block()._is_moveable)
                    this->get_current_state()[1]++;
                return value;
            }
            return this->get_current_block()._value;
        default:
            raise_error("Undefined move# " + to_string(action_id[0]));
        }
    }

    inline string move_tostring(const state_t<1>& move) const
    { string s[] = {"UP", "RIGHT", "DOWN", "LEFT"}; return s[move[0]]; }

    inline void set_random_state() {
        this->_state = {get_rand(0, this->size()[0]), get_rand(0, this->size()[1])};
    }
};

#endif // WORLD_MAZE_HPP
