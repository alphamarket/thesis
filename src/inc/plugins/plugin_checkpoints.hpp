#ifndef PLUGIN_CHECKPOINTS_HPP
#define PLUGIN_CHECKPOINTS_HPP

#include "inc/agent.plugins.hpp"

__DEFINE_PLUGIN__(checkpoints) {
    __PLUGIN_COMMONS__

    string name() const override { return "plugin_checkpoints"; }

    void notify_on_event(agent_t* const in) override {
        if(in) {
            assert(!in->world->get_maze()(in->_prev_state)._is_terminal);
            assert(in->world->get_maze()(in->_current_state)._is_moveable);
        } else raise_error("didn't expect that the agent's pointer be null!");
    }
};

#endif // PLUGIN_CHECKPOINTS_HPP
