#ifndef SEP_H
#define SEP_H

#include "stdafx.hpp"

class SEP
{

    typedef vector<size_t> state_t;

    size_t _steps;
    vector<size_t> _sizes;
    vector<vector<vector<scalar>>> _sep;
    vector<vector<vector<size_t>>> _shock;
    vector<vector<array<size_t, 4>>> _sep_sheet;
public:
    struct sep_detail {
        size_t  _action;
        size_t  _length;
        state_t
            _prev_state,
            _next_state;
    };

public:
    SEP(const vector<size_t>& sizes);
    void sep_reset();
    decltype(SEP::_sep) get_sep();
    void sep_visit_path(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state);
    void sep_dispatch_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state, const bool& current_is_goal = false);
};

#endif // SEP_H
