#ifndef AGENT_HPP
#define AGENT_HPP

#include "stdafx.hpp"

#include <future>
#include <unordered_map>

#include "iWorld.hpp"
#include "plugin.hpp"
#include "iQlearner.hpp"

template<size_t state_dim, size_t action_dim>
class agent {
    /**
     * @brief _instance_id current instance ID
     */
    const size_t _instance_id = 0;
    /**
     * @brief _instance_count Total number of instance this class been instantiated
     */
    static size_t _instance_count;

protected:
    /**
     * @brief The phases enum
     */
    enum phases_t { PHASE_ENTER = 0, PHASE_INIT, PHASE_PRE, PHASE_ON, PHASE_POST, PHASE_DEINIT, PHASE_EXIT };
    /**
     * @brief The action_advisor_method enum
     */
    enum action_advisor_method_t { BOLTZMANN = 0, GREEDY };
    /**
     * @brief _executing Check if the thread is executing or not?
     */
    bool
        _executing = 0,
        _threaded = 0;
    /**
     * @brief the learning params
     */
    scalar
        _tau,
        _beta,
        _gamma,
        _epsilon;
    /**
     * @brief _action_advisor_method The action advisor method
     */
    action_advisor_method_t _action_advisor_method;
    /**
     * @brief _thread The execution thread
     */
    future<void> _thread;
    /**
     * @brief _plugins The plugin map
     */
    vector<pair<string, plugin<state_dim, action_dim>* const>> _plugins;

public:
    /**
     * @brief _prev_q Previous Q value
     */
    scalar _prev_q = 0;
    /**
     * @brief _current_reward Current acquired reward
     */
    scalar _current_reward = 0;
    /**
     * @brief _prev_state The previous state
     */
    state_t<state_dim> _prev_state ;
    /**
     * @brief _prev_action The previous action
     */
    state_t<action_dim> _prev_action;
    /**
     * @brief _current_state The current state
     */
    state_t<state_dim> _current_state;
    /**
     * @brief _world The world that agent is operating in
     */
    IWorld<state_dim, action_dim>* world;
    /**
     * @brief _learner The learner the agent equipped
     */
    IQlearner<state_dim, action_dim>* learner;

protected:
    /**
     * @brief pluging_execute Executes plugings at each phase
     */
    void pluging_execute(const size_t phase) {
        switch(phase) {
        case PHASE_INIT: for(auto p : this->_plugins) p.second->notify_init_event(this); return;
        case PHASE_PRE: for(auto p : this->_plugins) p.second->notify_pre_event(this); return;
        case PHASE_ON: for(auto p : this->_plugins) p.second->notify_on_event(this); return;
        case PHASE_POST: for(auto p : this->_plugins) p.second->notify_post_event(this); return;
        case PHASE_DEINIT: for(auto p : this->_plugins) p.second->notify_deinit_event(this); return;
        case PHASE_ENTER: for(auto p : this->_plugins) p.second->notify_enter_event(this); return;
        case PHASE_EXIT: for(auto p : this->_plugins) p.second->notify_exit_event(this); return;
        default: raise_error("Undefined phase#" + to_string(phase));
        }
    }
    /**
     * @brief _execute The main agent executer wrapper
     * @param cycle The number of cycles to execute
     */
    void _execute(size_t cycle) {
        this->_executing = true;
        // engage the ENTER plugins
        this->pluging_execute(PHASE_ENTER);
        while (cycle--) {
            // engage the INIT plugins
            this->pluging_execute(PHASE_INIT);
            // while not reach to temination state
            while(!this->world->get_current_block()._is_terminal)
            {
                // store the prev state
                this->_prev_state = this->world->get_current_state();
                // engage the PRE plugins
                this->pluging_execute(PHASE_PRE);
                // pick an action considering the defined method
                switch(this->_action_advisor_method) {
                case BOLTZMANN:
                    // pick an action
                    this->_prev_action = this->learner->advise_action_boltzmann(this->world->get_current_state(), this->_tau);
                    break;
                case GREEDY:
                    // pick an action
                    this->_prev_action = this->learner->advise_action_greedy(this->world->get_current_state(), this->_epsilon);
                    break;
                default:
                    raise_error("Invalid action picker method `" + to_string(this->_action_advisor_method) + "`");
                }
                // move the action
                this->world->make_move(this->_prev_action);
                // get the reward
                this->_current_reward = this->world->get_current_block()._value;
                // get current state for the plugin call
                this->_current_state = this->world->get_current_state();
                // engage the ON plugins
                this->pluging_execute(PHASE_ON);
                // update the learner
                this->_prev_q = this->learner->update(this->_prev_state, this->_prev_action, this->world->get_current_state(), this->_current_reward, this->_beta, this->_gamma);
                // engage the POST plugins
                this->pluging_execute(PHASE_POST);
            }
            // engage the DEINIT plugins
            this->pluging_execute(PHASE_DEINIT);
        }
        this->_executing = false;
        // engage the EXIT plugins
        this->pluging_execute(PHASE_EXIT);
    }

public:
    agent(
        IWorld<state_dim, action_dim>* const world,
        IQlearner<state_dim, action_dim>* const learner,
        const string action_advisor_method = "boltzmann",
        const scalar beta = 0.01,
        const scalar gamma = .9,
        const scalar tau = .4,
        const scalar epsilon = .2)
        : _instance_id(++agent::_instance_count), _tau(tau), _beta(beta), _gamma(gamma), _epsilon(epsilon), world(world), learner(learner)
    {
        if(action_advisor_method == "boltzmann") this->_action_advisor_method = BOLTZMANN;
        else if(action_advisor_method == "greedy") this->_action_advisor_method = GREEDY;
        else raise_error("Invalid action picker method `" + action_advisor_method + "`");
    }
    /**
     * @brief operator += Adds new pluing
     */
    inline agent& operator+=(plugin<state_dim, action_dim>* const plugin)
    { this->_plugins.push_back({plugin->name(), plugin}); plugin->notify_add_event(this); return *this; }
    /**
     * @brief operator -= Removes a pluing
     */
    inline agent& operator-=(const string& plugin) {
        long e = -1;
        for(size_t i = 0; i < this->_plugins.size(); i++)
            if(this->_plugins[i].first == plugin) { e = i; break; }
        if(e >= 0) { this->_plugins[e].second->notify_add_event(this); this->_plugins.erase(e); }
        return *this;
    }
    /**
     * @brief get_plugin Get a pluging by name
     */
    template<template <size_t, size_t> class T>
    inline T<state_dim, action_dim>* get_plugin() const {
        const string plugin = T<state_dim, action_dim>().name();
        for(size_t i = 0; i < this->_plugins.size(); i++)
            if(this->_plugins[i].first == plugin)
                return static_cast<T<state_dim, action_dim>*>(this->_plugins.at(i).second);
        return nullptr;
    }
    /**
     * @brief get_plugins Get all of plugins
     */
    inline unordered_map<string, plugin<state_dim, action_dim>* const> get_plugins() const {
        unordered_map<string, plugin<state_dim, action_dim>* const> out;
        for(auto i : this->_plugins) out.insert({i.first, i.second});
        return out;
    }
    /**
     * @brief execute Executes the agent
     * @param cycle The cycle#
     * @param deattached should be run a thread detacthed to current thread?
     */
    void execute(const size_t cycle = 1, bool deattached = false) {
        auto exec = [this](size_t cycle) {
            this->_executing = true;
            try {
                this->_execute(cycle);
            } catch (...) {
                this->_executing = false;
                throw;
            }
            this->_executing = false;
        };
        _threaded = deattached;
        if(deattached) {
            if(this->_executing)
                raise_error("There is already a thread executing right now, cannot initiate new one!");
            this->_thread = std::async(std::launch::async, exec, cycle);
        }
        else
            exec(cycle);
    }
    /**
     * @brief wait_to_execute wait untill the agent get finish it's execution
     */
    void wait_to_execute()
    { if(this->_threaded) this->_thread.get(); }
};

template<size_t S, size_t T>
size_t agent<S, T>::_instance_count = 0;

#endif // AGENT_HPP
