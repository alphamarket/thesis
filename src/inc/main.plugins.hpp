#ifndef MAIN_PLUGINS_HPP
#define MAIN_PLUGINS_HPP

#include "stdafx.hpp"
#include "agent.hpp"
#include "iWorld.hpp"
#include "plugin.hpp"
#include "colormod.hpp"

#define __DEFINE_MAZE_PLUGIN__(x) class plugin_maze_##x : public plugin<2, 1>
#define __PLUGIN_MAZE_COMMONS__ typedef agent<2, 1> agent_t;

/**
 * @brief The print_maze_world class prints the final learned policy at the exist point
 */
__DEFINE_MAZE_PLUGIN__(print_policy) {
    __PLUGIN_MAZE_COMMONS__

    void notify_exit_event(agent_t* const in) override {
        auto p = in->learner->get_policy();
        cerr << "------------------" << endl;
        for(size_t i = 0; i < in->world->size()[0]; i++) {
            for(size_t j = 0; j < in->world->size()[1]; j++) {
                auto color = Color::Modifier(Color::FG_DEFAULT);
                auto ss = in->world->get_current_state();
                if(ss[0] == i && ss[1] == j) color = Color::Modifier(Color::FG_GREEN);
                if(!in->world->get_maze()[i][j]._is_moveable) color = Color::Modifier(Color::FG_RED);
                if(in->world->get_maze()[i][j]._value > 0) color = Color::Modifier(Color::FG_YELLOW);
                assert(in->world->get_current_block()._is_moveable);
                cerr << color << in->world->move_tostring({p({i, j})})[0] << " " << Color::Modifier(Color::FG_DEFAULT);
            }
            cerr << endl;
        }
        cerr << "***************" << endl;
    }
};\
/**
 * @brief The plugin_reset_agent class resets agents position at every cycle
 */
__DEFINE_MAZE_PLUGIN__(reset_agent) {
    __PLUGIN_MAZE_COMMONS__

    void notify_init_event(agent_t* const in) override {
        in->world->set_current_state(in->world->get_random_state());
    }
};

__DEFINE_MAZE_PLUGIN__(count_hop) {
    __PLUGIN_MAZE_COMMONS__
public:
    vector<size_t> hops;

    void notify_init_event(agent_t* const) override
    { hops.push_back(0); }

    void notify_on_event(agent_t* const) override
    { hops.back()++; }
};


#endif // MAIN_PLUGINS_Hpp
