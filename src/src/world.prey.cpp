#include "inc/world.prey.hpp"

void hunter_prey::update_current_state() {
    // reset if boundries are passed
    for(size_t i = 0; i < this->_loc_hunter.size(); i++) {
        this->_loc_prey[i] = max(min(this->_grid_size[i], this->_loc_prey[i]), 0.);
        this->_loc_hunter[i] = max(min(this->_grid_size[i], this->_loc_hunter[i]), 0.);
    }
    array<scalar, 2> d = { this->_loc_prey[0] - this->_loc_hunter[0], this->_loc_prey[1] - this->_loc_hunter[1] };
    size_t state = 0;
    if(abs(d[0]) < .5 && abs(d[1]) < .5) {
        if(d[0] >= 0 && d[1] >= 0)      state = 1;
        else if(d[0] >= 0 && d[1] <= 0) state = 5;
        else if(d[0] <= 0 && d[1] <= 0) state = 9;
        else if(d[0] <= 0 && d[1] >= 0) state = 13;
        else raise_error("the PC should not reach this!");
    }
    else if(abs(d[0]) < 1 && abs(d[1]) < 1) {
        if(d[0] >= 0 && d[1] >= 0)      state = 2;
        else if(d[0] >= 0 && d[1] <= 0) state = 6;
        else if(d[0] <= 0 && d[1] <= 0) state = 10;
        else if(d[0] <= 0 && d[1] >= 0) state = 14;
        else raise_error("the PC should not reach this!");
    }
    else if(abs(d[0]) < 1.5 && abs(d[1]) < 1.5) {
        if(d[0] >= 0 && d[1] >= 0)      state = 3;
        else if(d[0] >= 0 && d[1] <= 0) state = 7;
        else if(d[0] <= 0 && d[1] <= 0) state = 11;
        else if(d[0] <= 0 && d[1] >= 0) state = 15;
        else raise_error("the PC should not reach this!");
    }
    else if(abs(d[0]) <= 2 && abs(d[1]) <= 2) {
        if(d[0] >= 0 && d[1] >= 0)      state = 4;
        else if(d[0] >= 0 && d[1] <= 0) state = 8;
        else if(d[0] <= 0 && d[1] <= 0) state = 12;
        else if(d[0] <= 0 && d[1] >= 0) state = 16;
        else raise_error("the PC should not reach this!");
    }
    this->_state[0] = { state };
    assert(this->_state[0] <= 16);
}

scalar hunter_prey::make_move(const state_t<1> &action_id) {
    array<scalar, 2> hunters_speed = {0, 0};
    switch(action_id[0]) {
    case m0_05:
        hunters_speed = {0.5, 0.0};
        break;
    case m0_10:
        hunters_speed = {1.0, 0.0};
        break;
    case m45_05:
        hunters_speed = {0.5, 0.5};
        break;
    case m45_10:
        hunters_speed = {1.0, 1.0};
        break;
    case m90_05:
        hunters_speed = {0.0, 0.5};
        break;
    case m90_10:
        hunters_speed = {0.0, 1.0};
        break;
    case m135_05:
        hunters_speed = {-0.5, .5};
        break;
    case m135_10:
        hunters_speed = {-1.0, 1.};
        break;
    case m180_05:
        hunters_speed = {-0.5, 0.};
        break;
    case m180_10:
        hunters_speed = {-1.0, 0.};
        break;
    case m225_05:
        hunters_speed = {-.5, -.5};
        break;
    case m225_10:
        hunters_speed = {-1., -1.};
        break;
    case m270_05:
        hunters_speed = {0.0, -.5};
        break;
    case m270_10:
        hunters_speed = {0.0, -1.};
        break;
    case m315_05:
        hunters_speed = {0.5, -.5};
        break;
    case m315_10:
        hunters_speed = {1.0, -1.};
        break;
    default:
        raise_error("invalid action#" + to_string(action_id[0]));
    }
    this->_loc_prey = { this->_loc_prey[0] + frand() - 0.5, this->_loc_prey[1] + frand() - 0.5 };
    this->_loc_hunter = { this->_loc_hunter[0] + hunters_speed[0], this->_loc_hunter[1] + hunters_speed[1] };
    this->update_current_state();
    return this->get_current_block()._value;
}
