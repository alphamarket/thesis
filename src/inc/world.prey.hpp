#ifndef PREY_H
#define PREY_H

#include "iWorld.hpp"

class hunter_prey : public IWorld<1, 1>
{
protected:
    const size_t STATE_NO   = 17;
    const size_t ACTIONS_NO = 16;
    const array<scalar, 2> _grid_size = {10, 10};

    array<scalar, 2>
        _loc_hunter,
        _loc_prey;

    inline scalar get_angel(const array<scalar, 2>& d) const
    { scalar degree = atan2(d[0], d[1]) * 180 / M_PI; if(degree < 0) degree += 360; return degree; }

    void update_current_state();

public:
    // m<degree>_<step*10>
    enum moves { m0_05 = 0, m0_10, m45_05, m45_10, m90_05, m90_10, m135_05, m135_10, m180_05, m180_10, m225_05, m225_10, m270_05, m270_10, m315_05, m315_10 };

    hunter_prey()
        : base({17}, 16)
    {
        this->set_hunter_location({ scalar(get_rand(0, size_t(this->_grid_size[0]))) + frand(), scalar(get_rand(0, size_t(this->_grid_size[1]))) + frand() });
        this->set_prey_location({ scalar(get_rand(0, size_t(this->_grid_size[0]))) + frand(), scalar(get_rand(0, size_t(this->_grid_size[1]))) + frand() });
    }

    inline void set_prey_location(array<scalar, 2> _loc) { this->_loc_prey = _loc; this->update_current_state(); }
    inline void set_hunter_location(array<scalar, 2> _loc) { this->_loc_hunter = _loc; this->update_current_state(); }

    inline array<scalar, 2>& get_prey_location() { return this->_loc_prey; }
    inline array<scalar, 2>& get_hunter_location() { return this->_loc_hunter; }
    inline array<scalar, 2> get_prey_location() const { return this->_loc_prey; }
    inline array<scalar, 2> get_hunter_location() const { return this->_loc_hunter; }

    scalar make_move(const state_t<1>& action_id);

    inline string move_tostring(const state_t<1>& move) const
    {
        string s[] = {
            "d<0>, s<0.5>",
            "d<0>, s<1.0>",
            "d<45>, s<0.5>",
            "d<45>, s<1.0>",
            "d<90>, s<0.5>",
            "d<90>, s<1.0>",
            "d<135>, s<0.5>",
            "d<135>, s<1.0>",
            "d<180>, s<0.5>",
            "d<180>, s<1.0>",
            "d<225>, s<0.5>",
            "d<225>, s<1.0>",
            "d<270>, s<0.5>",
            "d<270>, s<1.0>",
            "d<315>, s<0.5>",
            "d<315>, s<1.0>",
        };
        return s[move[0]];
    }


    inline state_t<1> get_random_state() const
    { return {get_rand(0, this->size()[0])}; }
};

#endif // PREY_H
