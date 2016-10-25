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

protected:
    bool _executing;
    future<void> _thread;
    vector<IPlugin*> _plugins;
    IWorld<state_dim>* _world;
    IQlearner<state_dim, action_dim>* _learner;

protected:
    void _execute(size_t cycle) {
        this->_executing = true;
        while (cycle--) {
            printf("[%zu] %s cycle %zu\n", this->_instance_id, __FUNCTION__, cycle + 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        this->_executing = false;
    }

    inline std::future_status thread_status() const
    { return this->_thread.wait_for(std::chrono::seconds(0)); }

    inline bool thread_started() const
    { try { this->thread_status(); return true; } catch(...) { return false; } }

public:
    agent(IWorld<state_dim>* const world, IQlearner<state_dim, action_dim>* const learner)
        :  _instance_id(++agent::_instance_count), _world(world), _learner(learner)
    { }
    /**
     * @brief operator += Adds new pluing
     */
    inline agent& operator+=(IPlugin* const plugin) { this->_plugins.push_back(plugin); }
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
