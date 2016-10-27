#ifndef PLUGIN_RESET_AGENT_HPP
#define PLUGIN_RESET_AGENT_HPP

#include "inc/agent.plugins.hpp"

/**
 * @brief The plugin_reset_agent class resets agents position at every cycle
 */
__DEFINE_PLUGIN__(reset_agent) {
    __PLUGIN_COMMONS__
public:
    string name() const override { return "plugin_reset_agent"; }

    void notify_init_event(agent_t* const in) override
    {
        size_t i = in->world->get_maze().num_elements();
        while(i--) {
            auto s = in->world->get_random_state();
            if(!in->world->get_maze()(s)._is_terminal && in->world->get_maze()(s)._is_moveable) {
                in->world->set_current_state(s);
                return;
            }
        }
        raise_error("Timeout on setting the agent at right position!");
    }
};

#endif // PLUGIN_RESET_AGENT_HPP
