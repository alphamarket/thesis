#ifndef PLUGIN_MAZE_PRINT_POLICY_HPP
#define PLUGIN_MAZE_PRINT_POLICY_HPP

#include "inc/agent.plugins.hpp"

/**
 * @brief The plugin_print_policy class prints the final learned policy at the exist point
 */
__DEFINE_MAZE_PLUGIN__(print_policy) {
    __PLUGIN_MAZE_COMMONS__
public:
    string name() const override { return "plugin_print_policy"; }

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
};

#endif // PLUGIN_MAZE_PRINT_POLICY_HPP
