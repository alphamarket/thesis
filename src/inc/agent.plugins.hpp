#ifndef MAIN_PLUGINS_HPP
#define MAIN_PLUGINS_HPP

#include "stdafx.hpp"
#include "agent.hpp"
#include "iWorld.hpp"
#include "plugin.hpp"
#include "colormod.hpp"

#include <type_traits>

#define __DEFINE_PLUGIN__(x) template<size_t state_dim, size_t action_dim> class plugin_##x : public plugin<state_dim, action_dim>
#define __DEFINE_MAZE_PLUGIN__(x) class plugin_maze_##x : public plugin<2, 1>
#define __PLUGIN_MAZE_COMMONS__ typedef agent<2, 1> agent_t;
#define __PLUGIN_COMMONS__ typedef agent<state_dim, action_dim> agent_t;

/**
 * @brief The plugin_print_policy class prints the final learned policy at the exist point
 */
__DEFINE_MAZE_PLUGIN__(print_policy) {
    __PLUGIN_MAZE_COMMONS__
public:
    string name() const { return "plugin_print_policy"; }

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
/**
 * @brief The plugin_reset_agent class resets agents position at every cycle
 */
__DEFINE_PLUGIN__(reset_agent) {
    __PLUGIN_COMMONS__
public:
    string name() const { return "plugin_reset_agent"; }

    void notify_init_event(agent_t* const in) override
    { in->world->set_current_state(in->world->get_random_state()); }
};
/**
 * @brief The plugin_count_hop class counts the hops of agents taken
 */
__DEFINE_PLUGIN__(count_hop) {
    __PLUGIN_COMMONS__
public:
    vector<size_t> hops;

    string name() const { return "plugin_count_hop"; }

    void notify_init_event(agent_t* const) override
    { hops.push_back(0); }

    void notify_on_event(agent_t* const) override
    { hops.back()++; }
};
/**
 * @brief The plugin_SEP class The SEP method implementation
 */
__DEFINE_PLUGIN__(SEP) {
    __PLUGIN_COMMONS__

    struct cp_detail {
        state_t<state_dim> next_state;
        state_t<state_dim> prev_state;
        array<size_t, action_dim> action;
        long hop = -1;
        cp_detail () { }
        cp_detail(
            state_t<state_dim> next_state,
            state_t<state_dim> prev_state,
            array<size_t, action_dim> action,
            size_t hop) {
            this->next_state = next_state;
            this->prev_state = prev_state;
            this->action = action;
            this->hop = hop;
        }

    };

    matrix<cp_detail, state_dim>* cp;

public:

    matrix<scalar, state_dim + action_dim>* sep;
    matrix<size_t, state_dim + action_dim>* shock;

private:

    template<size_t ss, size_t aa>
    array<size_t, ss + aa> combine_sa(const array<size_t, ss>& s, const array<size_t, aa>& a) const {
        array<size_t, ss + aa> sa;
        size_t i = 0;
        for(; i < ss; i++) sa[i] = s[i];
        for(size_t j = 0; j < sa.size(); j++, i++) sa[i] = a[j];
        return sa;
    }

    void update_shock(agent_t* const in) {
        auto sa = this->combine_sa(in->_prev_state, in->_prev_action);
        auto cb = in->world->get_current_block();
        auto q = in->learner->get_actions_q(in->_current_state);
        if(accumulate(q.begin(), q.end(), 0.) > 0 || (cb._is_terminal && cb._value > 0))
            this->shock->operator ()(sa)++;
    }

    void update_cp(agent_t* const in)
    {
        // don't count the stay-put situations
        if(std::equal(in->_prev_state.begin(), in->_prev_state.end(), in->_current_state.begin())) return;
        this->cp->operator ()(in->_prev_state)
            = cp_detail(
                in->_current_state,
                in->_prev_state,
                in->_prev_action,
                ((plugin_count_hop<state_dim, action_dim>*)in->get_plugins()[plugin_count_hop<state_dim, action_dim>().name()])->hops.back());
    }

    vector<cp_detail> get_cp_details_sorted() {
        vector<cp_detail> cpd;
        std::for_each(
            this->cp->data(),
            this->cp->data() + this->cp->num_elements(),
            [&](cp_detail& c) {
                if(c.hop >= 0) cpd.push_back(c);
            });
        // sort it out the latest visited state should be at front
        sort(cpd.begin(), cpd.end(), [](auto i, auto j) { return i.hop > j.hop; });
        return cpd;
    }

    void update_sep(agent<state_dim, action_dim>* const) {
        auto cpd = this->get_cp_details_sorted();
        if(!cpd.size()) return;
        // make the step of the goal to zero
        // the next state of prev state should be the termination state
        auto ref = this->sep->slice_ref(slice<state_dim>(cpd.front().next_state));
        for_each(ref.data(), ref.data() + ref.num_elements(), [](const auto& i) { *i = 0; });
        for(size_t i = 0; i < cpd.size(); i++)
            this->sep->operator ()(this->combine_sa(cpd[i].prev_state, cpd[i].action)) = this->sep->slice(cpd[i].next_state).min() + 1;
    }
public:
    string name() const { return "plugin_SEP"; }

    void notify_deinit_event(agent_t* const in)
    { this->update_sep(in); }

    void notify_add_event(agent_t* const in) {
        if(!in->get_plugins().count(plugin_count_hop<state_dim, action_dim>().name()))
            raise_error("This plugin depends on `plugin_count_hop` plugin, but it is not manifested yet. Add the `plugin_count_hop` before this plugin.");
        this->sep = new matrix<scalar, state_dim + action_dim>(in->learner->Q.size());
        this->shock = new matrix<size_t, state_dim + action_dim>(in->learner->Q.size());

        array<size_t, state_dim> ssize;
        for(size_t i = 0; i < state_dim; i++) ssize[i] = in->learner->Q.size()[i];
        this->cp = new matrix<cp_detail, state_dim>(ssize);

        *this->shock = 0;
        *this->sep = numeric_limits<scalar>::infinity();
    }

    void notify_on_event(agent_t* const in) override {
        this->update_cp(in);
        this->update_shock(in);
    }

    ~plugin_SEP()
    { delete this->shock; delete this->cp; delete this->sep; }
};


#endif // MAIN_PLUGINS_Hpp
