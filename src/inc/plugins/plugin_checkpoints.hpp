#ifndef PLUGIN_CHECKPOINTS_HPP
#define PLUGIN_CHECKPOINTS_HPP

#include "inc/agent.plugins.hpp"
#include "inc/plugins/plugin_mce.hpp"

__DEFINE_PLUGIN__(checkpoints) {
    __PLUGIN_COMMONS__

public:
    plugin_checkpoints()
    { flag_workflow(); }

    string name() const override { return "plugin_checkpoints"; }

    void notify_on_event(agent_t* const in) override {
        if(in) {
            test1(in);
            test2(in);
        } else raise_error("didn't expect that the agent's pointer be null!");
    }

    void test2(agent_t* const in) {
        assert
            (!in->world->get_maze()(in->_prev_state)._is_terminal &&
             in->world->get_maze()(in->_current_state)._is_moveable);
    }

    void test1(agent_t* const in) {
        auto* mce = in->template get_plugin<plugin_MCE>();
        if(!mce) return;
        auto _max = mce->_CoQ.max();
        auto li = mce->_CoQ.template list_indices<state_dim>();
        for(auto ii : li) {
            mce->_CoQ.slice_ref(ii).for_each([_max](auto* i){
                auto test_converge = [&]() { return highpass(i); };
                return (!isnan(*i) && test_converge());
            });
        }
    }
};

#endif // PLUGIN_CHECKPOINTS_HPP
