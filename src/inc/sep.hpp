#ifndef SEP_H
#define SEP_H

#include "stdafx.hpp"

class SEP
{

    typedef vector<size_t> state_t;

    size_t _steps;
    vector<vector<vector<size_t>>> _shock;
    vector<vector<array<size_t, 4>>> _sep_sheet;
public:
    struct sep_detail {
        state_t _state;
        size_t  _action;
        size_t  _length;
    };

public:
    SEP(const vector<size_t>& sizes, const vector<state_t> &goals);
    vector<sep_detail> get_sep() const;
    void sep_reset(const vector<size_t>& sizes, const vector<state_t>& goals);
    void sep_visit_path(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state);
    void sep_dispatch_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state);
};

#endif // SEP_H
