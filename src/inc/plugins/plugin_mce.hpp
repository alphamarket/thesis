#ifndef PLUGIN_MCE_HPP
#define PLUGIN_MCE_HPP

#include "inc/stdafx.hpp"
#include "inc/agent.plugins.hpp"
#include "inc/plugins/plugin_count_hop.hpp"

/**
 * @brief The plugin_MCE class The `Multi-Criteria Expertness based Cooperative Learning in Multi-Agent Systems` method implementation
 */
__DEFINE_PLUGIN__(MCE) {
    __PLUGIN_COMMONS__

    typedef vector<scalar> points_t;

    typedef matrix<scalar, state_dim + action_dim> qtable_t;

public:

    qtable_t _CoQ;

protected:

    points_t
        /**
         * @brief _normal counts the normal sum of rewards
         */
        _normal,
        /**
         * @brief _positive counts the sum of positive rewards
         */
        _positive,
        /**
         * @brief _negative counts the sum of negative rewards
         */
        _negative,
        /**
         * @brief _distance holds the hops to reach the goal
         */
        _distance,
        /**
         * @brief _gradian counts gradian changes since the last cooperative learning cycle
         */
        _gradian,
        /**
         * @brief _absolute counts the absolute sum of rewards
         */
        _absolute;

    /**
     * @brief _criterias_all_in_one all criterias in one value
     */
    points_t _criterias_all_in_one;

protected:

    inline scalar calc_criteria_normal() const { return accumulate(this->_normal.begin(), this->_normal.end(), 0.); }

    inline scalar calc_criteria_gradian() const { assert(this->_gradian.size() == 2); return this->_gradian.back() - this->_gradian.front(); }

    inline scalar calc_criteria_positive() const { return accumulate(this->_positive.begin(), this->_positive.end(), 0.); }

    inline scalar calc_criteria_negative() const { return accumulate(this->_negative.begin(), this->_negative.end(), 0.); }

    inline scalar calc_criteria_absolute() const { return accumulate(this->_absolute.begin(), this->_absolute.end(), 0.); }

    inline scalar calc_criteria_avg_distance() const { return accumulate(this->_distance.begin(), this->_distance.end(), 0.) / this->_distance.size(); }

    inline void calc_criterias() {
        this->_criterias_all_in_one = {
            this->calc_criteria_normal(),
            this->calc_criteria_gradian(),
            this->calc_criteria_positive(),
            this->calc_criteria_negative(),
            this->calc_criteria_absolute(),
            this->calc_criteria_avg_distance()
        };
    }

public:

    inline points_t get_criterias() const { return this->_criterias_all_in_one; }

public:
    /**
     * @brief name name of current plugin
     */
    string name() const { return "plugin_MCE"; }
    /**
     * @brief plugin_reference_counter ctor
     */
    plugin_MCE()
    { flag_workflow(); }
    /**
     * @brief notify_add_event initializes the plugin variables according to the agent
     */
    void notify_add_event(agent_t* const in)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        this->_CoQ.resize(in->learner->Q.size());
        this->_gradian.push_back(0);
        this->_CoQ = 0;
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
    /**
     * @brief notify_enter_event on every learning cycle start we forward the gradian counters
     */
    void notify_enter_event(agent_t* const)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        this->_normal.clear();
        this->_absolute.clear();
        this->_negative.clear();
        this->_positive.clear();
        this->_distance.clear();
        this->_gradian = { this->_gradian.back() };
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
    /**
     * @brief notify_init_event at the begining of each cycle, re-init the variables
     */
    void notify_init_event(agent_t* const)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        this->_normal.push_back(0);
        this->_positive.push_back(0);
        this->_negative.push_back(0);
        this->_absolute.push_back(0);
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
    /**
     * @brief notify_on_event on every move update the ref counter
     */
    void notify_on_event(agent_t* const in)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        // pick from CoQ table using boltzmann

        // pick a random action
        in->_prev_action = {in->learner->advise_boltzmann(this->_CoQ.slice(in->_prev_state), in->_tau)};
        // move the action
        in->world->make_move(in->_prev_action);
        // get the reward
        in->_current_reward = in->world->get_current_block()._value;
        // get current state for the plugin call
        in->_current_state = in->world->get_current_state();

        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        // update the criterias

        // update the normal criteria
        this->_normal.back() += in->_current_reward;
        // update the absolute criteria
        this->_absolute.back() += abs(in->_current_reward);

        if(in->_current_reward > 0)
            // update the positive criteria
            this->_positive.back() += in->_current_reward;
        else
            // update the negative criteria
            this->_negative.back() += -in->_current_reward;
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
    /**
     * @brief notify_deinit_event after each cycle, recall or update the recall memory
     */
    void notify_deinit_event(agent_t* const in)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        this->_distance.push_back(in->template get_plugin<plugin_count_hop>()->hops.back());
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
    /**
     * @brief notify_exit_event printing the reference counts at the end of learning cycles
     */
    void notify_exit_event(agent_t* const)
    {
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
        this->_gradian.push_back(this->_normal.back());
        this->calc_criterias();
        cerr << __FUNCTION__ << " [ " << __LINE__ << " ]\n";
    }
};

#endif // PLUGIN_MCE_HPP
