#ifndef PLUGIN_SEP_HPP
#define PLUGIN_SEP_HPP

#include "inc/agent.plugins.hpp"
#include "inc/plugins/plugin_count_hop.hpp"

/**
 * @brief The plugin_SEP class The SEP method implementation
 */
__DEFINE_PLUGIN__(SEP) {
    __PLUGIN_COMMONS__

    class cp_detail {
    public:
        state_t<state_dim> next_state;
        state_t<state_dim> prev_state;
        array<size_t, action_dim> action;
        size_t hop = 0;
        bool initialized = false;
        cp_detail () { }
        cp_detail(
            state_t<state_dim> next_state,
            state_t<state_dim> prev_state,
            array<size_t, action_dim> action,
            size_t hop)
            : initialized(true)
        {
            this->next_state = next_state;
            this->prev_state = prev_state;
            this->action = action;
            this->hop = hop;
        }
        cp_detail(const cp_detail& cp)
        { this->operator =(cp); }

        cp_detail(cp_detail&& cp)
            : initialized(move(cp.initialized))
        {
            this->hop = move(cp.hop);
            this->action = move(cp.action);
            this->next_state = move(cp.next_state);
            this->prev_state = move(cp.prev_state);
        }
        cp_detail& operator = (const cp_detail& cp)
        {
            this->hop = cp.hop;
            this->initialized = cp.initialized;
            copy(cp.action.begin(), cp.action.end(), this->action.begin());
            copy(cp.next_state.begin(), cp.next_state.end(), this->next_state.begin());
            copy(cp.prev_state.begin(), cp.prev_state.end(), this->prev_state.begin());
            return *this;
        }
    };

    vector<cp_detail> _pathes;
    state_t<state_dim> _cp_size;
    matrix<cp_detail, state_dim> _cp;

public:

    matrix<scalar, state_dim + action_dim> sep;
    matrix<size_t, state_dim + action_dim> shock;

private:

    void update_shock(agent_t* const in) {
        auto cb = in->world->get_current_block();
        auto q = in->learner->get_actions_q(in->_current_state);
        auto sa = in->learner->combine_state_action(in->_prev_state, in->_prev_action);
        // if the current state is terminal and goal
        // or there is already a shock value in this state
        if(accumulate(q.begin(), q.end(), 0.) > 0 || (cb._is_terminal && cb._value > 0))
            // increament the shock
            this->shock.operator ()(sa)++;
    }

    void update_cp(agent_t* const in) {
        this->_cp.operator ()(in->_prev_state)
            = cp_detail(
                in->_current_state,
                in->_prev_state,
                in->_prev_action,
                ((plugin_count_hop<state_dim, action_dim>*)in->get_plugins()[plugin_count_hop<state_dim, action_dim>().name()])->hops.back());
        // track the path
        this->_pathes.push_back(this->_cp.operator ()(in->_prev_state));
    }

    void update_sep(agent_t* const in) {
        vector<cp_detail> cpd;
        // add the initialized items to the checking manifest
        this->_cp.for_each([&cpd](cp_detail& c) { if(c.initialized) cpd.push_back(c); });
        // sort it out the latest visited state should be at front
        sort(cpd.begin(), cpd.end(), [](auto i, auto j) { return i.hop > j.hop; });
        // if there is not any detials? do nothing
        if(!cpd.size()) return;
        // make the step of the goal to zero
        // the next state of prev state should be the termination state
        this->sep.slice_ref(slice<state_dim>(cpd.front().next_state)).for_each([](auto* i) { *i = 0; });
        // some fail checks
        assert(this->sep.slice(slice<state_dim>(cpd.front().next_state)) == 0);
        assert(in->world->get_maze()(this->_pathes.back().next_state)._is_terminal && in->world->get_maze()(cpd.front().next_state)._is_terminal);
        // for all cp details
        for(size_t i = 0; i < cpd.size(); i++) {
            // we should not meet `inf` at this point
            assert(this->sep.slice(cpd[i].next_state).min() != numeric_limits<scalar>::infinity());
            // update the sep value for prev state/action
            this->sep.operator ()(in->learner->combine_state_action(cpd[i].prev_state, cpd[i].action)) = this->sep.slice(cpd[i].next_state).min() + 1;
        }
    }

public:
    string name() const override { return "plugin_SEP"; }

    void notify_deinit_event(agent_t* const in) override
    { this->update_sep(in); }

    void notify_add_event(agent_t* const in) override {
        if(!in->template get_plugin<plugin_count_hop>())
            raise_error("This plugin depends on `plugin_count_hop` plugin, but it is not manifested yet. Add the `plugin_count_hop` before this plugin.");
        // reform the matrices to init them
        this->sep.resize(in->learner->Q.size());
        this->shock.resize(in->learner->Q.size());
        // init them
        this->shock = 0;
        this->sep = numeric_limits<scalar>::infinity();
        // init the update path size
        copy_n(in->learner->Q.size().begin(), state_dim, this->_cp_size.begin());
        // resize it for later initialzation
        this->_cp.resize(this->_cp_size);
    }

    void notify_on_event(agent_t* const in) override {
        // don't count the stay-put situations
        if(std::equal(in->_prev_state.begin(), in->_prev_state.end(), in->_current_state.begin())) return;
        // update the current path matrix
        this->update_cp(in);
        // update the shock matrix
        this->update_shock(in);
    }

    void notify_init_event(agent_t* const) override {
        this->_pathes.clear();
        this->_cp = matrix<cp_detail, state_dim>(this->_cp_size);
    }
};

#endif // PLUGIN_SEP_HPP
