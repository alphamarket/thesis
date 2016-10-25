#ifndef AGENT_HPP
#define AGENT_HPP

#include "stdafx.hpp"

#include <future>

#include "iWorld.hpp"
#include "iPlugin.hpp"
#include "iQlearner.hpp"

template<size_t state_dim, size_t action_dim>
class agent {
    friend class IPlugin;
    const size_t _instance_id = 0;
    static size_t _instance_count;

    enum phases { PHASE_INIT = 0, PHASE_PRE, PHASE_ON, PHASE_POST, PHASE_DEINIT, PHASE_ENTER, PHASE_EXIT };
protected:
    bool _executing;
    future<void> _thread;
    vector<IPlugin*> _plugins;
public:
    IWorld<state_dim, action_dim>* _world;
    IQlearner<state_dim, action_dim>* _learner;

protected:
    void pluging_execute(const size_t phase) {
        switch(phase) {
        case PHASE_INIT: foreach_elem(p, this->_plugins) p->notify_init_event(this); return;
        case PHASE_PRE: foreach_elem(p, this->_plugins) p->notify_pre_event(this); return;
        case PHASE_ON: foreach_elem(p, this->_plugins) p->notify_on_event(this); return;
        case PHASE_POST: foreach_elem(p, this->_plugins) p->notify_post_event(this); return;
        case PHASE_DEINIT: foreach_elem(p, this->_plugins) p->notify_deinit_event(this); return;
        case PHASE_ENTER: foreach_elem(p, this->_plugins) p->notify_enter_event(this); return;
        case PHASE_EXIT: foreach_elem(p, this->_plugins) p->notify_exit_event(this); return;
        default: error("Undefined phase#" + to_string(phase));
        }
    }

    void _execute(size_t cycle) {
        this->_executing = true;
        this->pluging_execute(PHASE_ENTER);
        while (cycle--) {
            this->pluging_execute(PHASE_INIT);
            while(!this->_world->get_current_block()._is_terminal)
            {
                auto prev_state = this->_world->get_current_state();
                this->pluging_execute(PHASE_PRE);
                auto action = this->_learner->advise_action_boltzmann(this->_world->get_current_state(), .4);
                this->_world->make_move(action);
                this->pluging_execute(PHASE_ON);
                auto reward = this->_world->get_current_block()._value;
                this->_learner->update(prev_state, action, this->_world->get_current_state(), reward, .01, .9);
                this->pluging_execute(PHASE_POST);
            }
            this->pluging_execute(PHASE_DEINIT);
        }
        this->_executing = false;
        this->pluging_execute(PHASE_EXIT);
    }

    inline std::future_status thread_status() const
    { return this->_thread.wait_for(std::chrono::seconds(0)); }

    inline bool thread_started() const
    { try { this->thread_status(); return true; } catch(...) { return false; } }

public:
    agent(IWorld<state_dim, action_dim>* const world, IQlearner<state_dim, action_dim>* const learner)
        :  _instance_id(++agent::_instance_count), _world(world), _learner(learner)
    { }
    /**
     * @brief operator += Adds new pluing
     */
    inline agent& operator+=(IPlugin* const plugin) { this->_plugins.push_back(plugin); return *this; }
    /**
     * @brief operator += Removes a pluing
     */
    inline agent& operator-=(IPlugin* const plugin)
    {
        size_t e = 0;
        std::all_of(
            this->_plugins.begin(),
            this->_plugins.end(),
            [&](const auto* i) {
                if(plugin == i) {
                    this->_plugins.erase(e);
                    return false;
                }
                e++;
                return true;
        });
        return *this;
    }
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
            if(this->thread_started() && this->thread_status() != future_status::ready)
                error("There is already a thread executing right now, cannot initiate new one!");
            this->_thread = std::async(std::launch::async, exec, cycle);
        }
        else
            exec(cycle);
    }
    void wait_to_execute()
    { if(this->_executing) this->_thread.get(); }
};

template<size_t S, size_t T>
size_t agent<S, T>::_instance_count = 0;

#endif // AGENT_HPP
