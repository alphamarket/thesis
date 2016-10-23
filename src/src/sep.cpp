#include "inc/sep.hpp"
#include <limits>

SEP::SEP(const vector<size_t>& sizes, const vector<state_t>& goals)
    : _steps(0)
{
    this->sep_reset(sizes, goals);
}

void SEP::sep_reset(const vector<size_t>& sizes, const vector<state_t>& goals) {
    this->_steps = 0;
    assert(sizes.size() == 3);
    this->_shock.clear();
    this->_sep_sheet.clear();
    for(size_t i = 0; i < sizes[0]; i++) {
        this->_shock.push_back(vector<vector<size_t>>());
        this->_sep_sheet.push_back(vector<array<size_t, 4>>());
        for(size_t j = 0; j < sizes[1]; j++) {
            this->_shock[i].push_back(vector<size_t>());
            this->_sep_sheet[i].push_back({0, 0, 0, numeric_limits<size_t>::infinity()});
            for(size_t k = 0; k < sizes[2]; k++) {
                this->_shock[i][j].push_back(0);
            }
        }
    }
    foreach_elem(g, goals) {
        foreach_elem(&a, this->_shock[g[0]][g[1]]) {
            a = 1;
        }
    }
}


void SEP::sep_dispatch_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state) {
    assert(curr_state.size() == prev_state.size() && prev_state.size() == 2);
    auto actions = this->_shock[curr_state[0]][curr_state[1]];
    if(accumulate(actions.begin(), actions.end(), 0.) > 0)
        this->_shock[prev_state[0]][prev_state[1]][prev_action]++;
}

void SEP::sep_visit_path(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state) {
    this->_sep_sheet[prev_state[0]][prev_state[1]] = { curr_state[0], curr_state[1], prev_action, ++this->_steps };
}


vector<SEP::sep_detail> SEP::get_sep() const {
    vector<sep_detail> out;
    for(size_t i = 0; i < this->_sep_sheet.size(); i++)
        for(size_t j = 0; j < this->_sep_sheet[i].size(); j++)
            if(this->_sep_sheet[i][j][3] != numeric_limits<size_t>::infinity())
                out.push_back({{i, j}, this->_sep_sheet[i][j][2], this->_sep_sheet[i][j][3] });

    sort(out.begin(), out.end(), [](auto i, auto j) { return i._length > j._length; });
    size_t i = 1;
    foreach_elem(&e, out) e._length = i++;
    return out;
}
