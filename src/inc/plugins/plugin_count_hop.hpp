#ifndef PLUGIN_COUNT_HOP_HPP
#define PLUGIN_COUNT_HOP_HPP

#include "inc/agent.plugins.hpp"

/**
 * @brief The plugin_count_hop class counts the hops of agents taken
 */
__DEFINE_PLUGIN__(count_hop) {
    __PLUGIN_COMMONS__
public:
    vector<size_t> hops;

    string name() const override { return "plugin_count_hop"; }

    void notify_init_event(agent_t* const) override
    { hops.push_back(0); }

    void notify_on_event(agent_t* const) override
    { hops.back()++; }
};

#endif // PLUGIN_COUNT_HOP_HPP
