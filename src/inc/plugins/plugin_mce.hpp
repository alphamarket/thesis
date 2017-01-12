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

    typedef scalar points_t;

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
    vector<points_t> _criterias_all_in_one;

public:

    inline vector<points_t> get_criterias() const { return this->_criterias_all_in_one; }

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
        this->_CoQ.resize(in->learner->Q.size());
        this->_CoQ = 0;
    }
    /**
     * @brief notify_enter_event on every learning cycle start we forward the gradian counters
     */
    void notify_enter_event(agent_t* const)
    { this->_gradian = 0; }
    /**
     * @brief notify_on_event on every move update the ref counter
     */
    void notify_on_event(agent_t* const in)
    {
        // pick from CoQ table using boltzmann
        // reset the agent's location to its previous location
        in->world->set_current_state(in->_prev_state);
        // pick a random action
        in->_prev_action = {in->learner->advise_boltzmann(this->_CoQ.slice(in->_prev_state), in->_tau)};
        // move the action
        in->world->make_move(in->_prev_action);
        // get the reward
        in->_current_reward = in->world->get_current_block()._value;
        // get current state for the plugin call
        in->_current_state = in->world->get_current_state();

        this->_normal += in->_current_reward;
        this->_gradian += in->_current_reward;
        this->_absolute += abs(in->_current_reward);
        this->_positive += (in->_current_reward > 0 ? in->_current_reward : 0);
        this->_negative += (in->_current_reward < 0 ? -in->_current_reward : 0);
    }
    /**
     * @brief notify_exit_event printing the reference counts at the end of learning cycles
     */
    void notify_exit_event(agent_t* const in)
    {
        auto* hops = &in->template get_plugin<plugin_count_hop>()->hops;
        this->_criterias_all_in_one = {
            this->_normal,
            this->_gradian,
            this->_absolute,
            this->_positive,
            this->_negative,
            1. / (accumulate(hops->begin(), hops->end(), 0.0) / hops->size())
        };
    }
};

#endif // PLUGIN_MCE_HPP
