#ifndef MAIN_PLUGINS_HPP
#define MAIN_PLUGINS_HPP

#include "stdafx.hpp"
#include "agent.hpp"
#include "iWorld.hpp"
#include "plugin.hpp"

#include <type_traits>

#define __DEFINE_PLUGIN__(x) \
    template<size_t state_dim, size_t action_dim> \
    class plugin_##x : public plugin<state_dim, action_dim>

#define __DEFINE_MAZE_PLUGIN__(x) \
    class plugin_maze_##x : public plugin<2, 1>

#define __PLUGIN_MAZE_COMMONS__ typedef agent<2, 1> agent_t;

#define __PLUGIN_COMMONS__ typedef agent<state_dim, action_dim> agent_t;

#endif // MAIN_PLUGINS_Hpp
