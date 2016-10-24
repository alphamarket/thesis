#include "inc/sep.hpp"
#include <limits>
#include <unordered_map>

SEP::SEP(const vector<size_t>& sizes)
    : _steps(0), _sizes(sizes)
{
    this->sep_reset();
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

void SEP::sep_dispatch_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state, const bool& current_is_goal) {
    assert(curr_state.size() == prev_state.size() && prev_state.size() == 2);
    auto actions = this->_shock[curr_state[0]][curr_state[1]];
    if(accumulate(actions.begin(), actions.end(), 0.) > 0 || current_is_goal)
        this->_shock[prev_state[0]][prev_state[1]][prev_action]++;
}

void SEP::sep_visit_path(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state) {
    this->_sep_sheet[prev_state[0]][prev_state[1]] = { curr_state[0], curr_state[1], prev_action, ++this->_steps };
}

decltype(SEP::_sep) SEP::get_sep() {
    vector<sep_detail> out;
    auto min_vec = [](const vector<scalar>& v) { return *min_element(v.begin(), v.end()); };
    for(size_t i = 0; i < this->_sep_sheet.size(); i++) {
        for(size_t j = 0; j < this->_sep_sheet[i].size(); j++) {
            if(this->_sep_sheet[i][j][3] < numeric_limits<size_t>::max()) {
                out.push_back({this->_sep_sheet[i][j][2], this->_sep_sheet[i][j][3], {i, j}, {this->_sep_sheet[i][j][0], this->_sep_sheet[i][j][1]}});
            }
        }
    }
    if(!out.size()) return this->_sep;
    sort(out.begin(), out.end(), [](auto i, auto j) { return i._length > j._length; });
    this->_sep[out.front()._prev_state[0]][out.front()._prev_state[1]][out.front()._action] = 1;
    for(size_t i = 1; i < out.size(); i++) {
        this->_sep[out[i]._prev_state[0]][out[i]._prev_state[1]][out[i]._action]
            = min_vec(this->_sep[out[i]._next_state[0]][out[i]._next_state[1]]) + 1;
    }
    return this->_sep;
}
