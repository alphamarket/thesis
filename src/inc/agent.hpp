#ifndef AGENT_HPP
#define AGENT_HPP

#include "stdafx.hpp"

#include <future>
#include <unordered_map>

#include "iWorld.hpp"
#include "iPlugin.hpp"
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

    enum phases { PHASE_INIT = 0, PHASE_PRE, PHASE_ON, PHASE_POST, PHASE_DEINIT, PHASE_ENTER, PHASE_EXIT };
protected:
    /**
     * @brief _executing Check if the thread is executing or not?
     */
    bool _executing;
    /**
     * @brief _thread The execution thread
     */
    future<void> _thread;
    /**
     * @brief _plugins The plugin map
     */
    unordered_map<string, IPlugin* const> _plugins;
public:
    /**
     * @brief _prev_q Previous Q value
     */
    scalar _prev_q;
    /**
     * @brief _current_reward Current acquired reward
     */
    scalar _current_reward;
    /**
     * @brief _prev_state The previous state
     */
    state_t<state_dim> _prev_state;
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
        case PHASE_INIT: foreach_elem(p, this->_plugins) p.second->notify_init_event(this); return;
        case PHASE_PRE: foreach_elem(p, this->_plugins) p.second->notify_pre_event(this); return;
        case PHASE_ON: foreach_elem(p, this->_plugins) p.second->notify_on_event(this); return;
        case PHASE_POST: foreach_elem(p, this->_plugins) p.second->notify_post_event(this); return;
        case PHASE_DEINIT: foreach_elem(p, this->_plugins) p.second->notify_deinit_event(this); return;
        case PHASE_ENTER: foreach_elem(p, this->_plugins) p.second->notify_enter_event(this); return;
        case PHASE_EXIT: foreach_elem(p, this->_plugins) p.second->notify_exit_event(this); return;
        default: error("Undefined phase#" + to_string(phase));
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
                // pick an action
                this->_prev_action = this->learner->advise_action_boltzmann(this->world->get_current_state(), .4);
                // move the action
                this->world->make_move(this->_prev_action);
                // get the reward
                this->_current_reward = this->world->get_current_block()._value;
                // engage the ON plugins
                this->pluging_execute(PHASE_ON);
                // update the learner
                this->_prev_q = this->learner->update(this->_prev_state, this->_prev_action, this->world->get_current_state(), this->_current_reward, .01, .9);
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
    agent(IWorld<state_dim, action_dim>* const world, IQlearner<state_dim, action_dim>* const learner)
        :  _instance_id(++agent::_instance_count), world(world), learner(learner)
    { }
    /**
     * @brief operator += Adds new pluing
     */
    inline agent& operator+=(pair<string, IPlugin* const> plugin) { this->_plugins.insert(plugin); return *this; }
    /**
     * @brief operator += Removes a pluing
     */
    inline agent& operator-=(const string& plugin) { this->_plugins.erase(plugin); return *this; }
    /**
     * @brief get_plugin Get a pluging by name
     */
    inline IPlugin* get_plugin(const string& plugin) const { return this->_plugins[plugin]; }
    /**
     * @brief get_plugins Get all of plugins
     */
    inline unordered_map<string, IPlugin* const> get_plugins() const { return this->_plugins; }
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
        if(deattached) {
            if(this->_executing)
                error("There is already a thread executing right now, cannot initiate new one!");
            this->_thread = std::async(std::launch::async, exec, cycle);
        }
        else
            exec(cycle);
    }
    /**
     * @brief wait_to_execute wait untill the agent get finish it's execution
     */
    void wait_to_execute()
    { if(this->_executing) this->_thread.get(); }
};

template<size_t S, size_t T>
size_t agent<S, T>::_instance_count = 0;

#endif // AGENT_HPP
