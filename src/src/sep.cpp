#include "inc/sep.hpp"
#include <limits>
#include <unordered_map>

SEP::SEP(const vector<size_t>& sizes)
    : _steps(0), _sizes(sizes)
{
    this->sep_reset();
}

SEP& SEP::operator =(const pair<sepmat_t, shockmat_t>& init_pair) {
    assert(this->_sizes[0] == init_pair.first.size() && this->_sizes[0] == init_pair.second.size());
    for(size_t i = 0; i < this->_sizes[0]; i++) {
        assert(this->_sizes[1] == init_pair.first[i].size() && this->_sizes[1] == init_pair.second[i].size());
        for(size_t j = 0; j < this->_sizes[1]; j++)
            assert(this->_sizes[2] == init_pair.first[i][j].size() && this->_sizes[2] == init_pair.second[i][j].size());
    }
    this->sep_reset();
    this->_sep = init_pair.first;
    this->_shock = init_pair.second;
    return *this;
}

void SEP::sep_reset() {
    this->_steps = 0;
    assert(this->_sizes.size() == 3);
    this->_sep.clear();
    this->_shock.clear();
    this->_sep_sheet.clear();
    for(size_t i = 0; i < this->_sizes[0]; i++) {
        this->_sep.push_back(vector<vector<scalar>>());
        this->_shock.push_back(vector<vector<size_t>>());
        this->_sep_sheet.push_back(vector<array<size_t, 4>>());
        for(size_t j = 0; j < this->_sizes[1]; j++) {
            this->_sep[i].push_back(vector<scalar>());
            this->_shock[i].push_back(vector<size_t>());
            this->_sep_sheet[i].push_back({0, 0, 0, numeric_limits<size_t>::max()});
            for(size_t k = 0; k < this->_sizes[2]; k++)
            { this->_sep[i][j].push_back(numeric_limits<scalar>::infinity()); this->_shock[i][j].push_back(0); }
        }
    }
}

void SEP::sep_update_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state, const bool& current_is_goal) {
    assert(curr_state.size() == prev_state.size() && prev_state.size() == 2);
    auto actions = this->_shock[curr_state[0]][curr_state[1]];
    // if we have the at least any +1 value at current state or current state is the goal state
    if(accumulate(actions.begin(), actions.end(), 0.) > 0 || current_is_goal)
        // increment the prev state
        this->_shock[prev_state[0]][prev_state[1]][prev_action]++;
}

void SEP::sep_update_sep() {
    vector<sep_detail> out;
    auto min_vec = [](const vector<scalar>& v) { return *min_element(v.begin(), v.end()); };
    // collect the info about the states it went through
    for(size_t i = 0; i < this->_sep_sheet.size(); i++) {
        for(size_t j = 0; j < this->_sep_sheet[i].size(); j++) {
            if(this->_sep_sheet[i][j][3] < numeric_limits<size_t>::max()) {
                out.push_back({this->_sep_sheet[i][j][2], this->_sep_sheet[i][j][3], {i, j}, {this->_sep_sheet[i][j][0], this->_sep_sheet[i][j][1]}});
            }
        }
    }
    if(!out.size()) return;
    // sort it out the latest visited state should be at front
    sort(out.begin(), out.end(), [](auto i, auto j) { return i._length > j._length; });
    // make the step of the goal to zero
    // the next state of prev state should be the termination state
    for(size_t i = 0; i < this->_sizes[2]; i++) this->_sep[out.front()._next_state[0]][out.front()._next_state[1]][i] = 0;
    // for all prev states
    for(size_t i = 0; i < out.size(); i++) {
        // we should not meet `inf` at this point
        assert(min_vec(this->_sep[out[i]._next_state[0]][out[i]._next_state[1]]) != numeric_limits<scalar>::infinity());
        // minimum of the next state length + 1 is the SEP of prev state!
        this->_sep[out[i]._prev_state[0]][out[i]._prev_state[1]][out[i]._action]
            = min_vec(this->_sep[out[i]._next_state[0]][out[i]._next_state[1]]) + 1;
    }
}
