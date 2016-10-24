#ifndef SEP_H
#define SEP_H

#include "stdafx.hpp"

class SEP
{
public:
    typedef vector<vector<vector<scalar>>> sepmat_t;
    typedef vector<vector<vector<size_t>>> shockmat_t;

private:
    typedef vector<size_t> state_t;
    /**
     * @brief _steps Counts the steps taken
     */
    size_t _steps;
    /**
     * @brief _sizes The vector of sizes
     */
    vector<size_t> _sizes;
    /**
     * @brief _sep The SEP matrix
     */
    sepmat_t _sep;
    /**
     * @brief _shock The SHOCK matrix
     */
    shockmat_t _shock;
    /**
     * @brief _sep_sheet The working SEP sheet
     */
    vector<vector<array<size_t, 4>>> _sep_sheet;
    /**
     * @brief The sep detail struct
     */
    struct sep_detail {
        size_t  _action;
        size_t  _length;
        state_t
            _prev_state,
            _next_state;
    };

    inline bool is_equal(const state_t& s1, const state_t& s2) {
        if(s1.size()!=s2.size()) return false;
        for(size_t i = 0; i < s1.size(); i++)
            if(s1[i]!=s2[i]) return false;
        return true;
    }

public:
    /**
     * @brief SEP Construct the SEP/SHOCK matrix
     * @param sizes The sizes of {width, height, actions}
     */
    SEP(const vector<size_t>& sizes);
    /**
     * @brief sep_reset Resets the SEP params
     */
    void sep_reset();
    /**
     * @brief get_update_sep Updates the SEP matrix
     */
    void sep_update_sep();
    /**
     * @brief sep_get_sep Gets the SEP matrix
     * @return
     */
    inline sepmat_t sep_get_sep() const { return this->_sep; }
    /**
     * @brief get_sep Gets the SHOCK matrix
     */
    inline shockmat_t sep_get_shock() const { return this->_shock; }
    /**
     * @brief sep_visit_path Flag the SEP that a state has been visited
     * @param prev_state The previous state
     * @param prev_action The token action
     * @param curr_state The current state
     */
    inline void sep_visit_path(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state)
    { if(this->is_equal(prev_state, curr_state)) return; this->_sep_sheet[prev_state[0]][prev_state[1]] = { curr_state[0], curr_state[1], prev_action, ++this->_steps }; }
    /**
     * @brief sep_dispatch_shock Flag the SHOCK to dipatch values if possible
     * @param prev_state The previous state
     * @param prev_action The token action
     * @param curr_state The current state
     * @param current_is_goal If the current state is the goal state
     */
    void sep_update_shock(const state_t& prev_state, const size_t& prev_action, const state_t& curr_state, const bool& current_is_goal = false);
    /**
     * @brief operator = Initialize the current instance with SEP & SHOCK matrices
     * @param init_pair The init pairs of SEP & SHOCK matrices
     */
    SEP& operator =(const pair<const sepmat_t&, const shockmat_t&>& init_pair);
};

#endif // SEP_H
