#ifndef PLUGIN_REFMAT_HPP
#define PLUGIN_REFMAT_HPP

#include "inc/stdafx.hpp"
#include "inc/agent.plugins.hpp"

/**
 * @brief The plugin_reference_counter class The reference counter method implementation
 */
__DEFINE_PLUGIN__(reference_counter) {
    __PLUGIN_COMMONS__

    typedef matrix<size_t, state_dim> refmat_t;
    /**
     * @brief _recall the recall matrix
     */
    refmat_t _recall;
    /**
     * @brief _grind The grind value
     */
    const size_t _grind;
    /**
     * @brief _dim The grind dimension
     */
    state_t<state_dim> _dim;
    /**
     * @brief arr_divide divides a state into divisions
     */
    template<size_t din>
    state_t<state_dim> arr_divide(const state_t<din>& s, const size_t& d) {
        state_t<state_dim> out;
        assert(state_dim <= din);
        for(size_t i = 0; i < state_dim; i++) out[i] = s[i] / d;
        return out;
    }

public:
    /**
     * @brief refmat reference count matrix
     */
    refmat_t refmat;
    /**
     * @brief name name of current plugin
     */
    string name() const { return "plugin_reference_counter"; }
    /**
     * @brief plugin_reference_counter defines the grind value
     */
    plugin_reference_counter(const size_t& grind = 1)
        : _grind(grind)
    { flag_workflow(); }
    /**
     * @brief ref_state returns the reference state of given state according to the define grin
     */
    inline state_t<state_dim> ref_state(const state_t<state_dim>& s)
    { return this->arr_divide(s, this->_grind); }
    /**
     * @brief notify_add_event initializes the plugin variables according to the agent
     */
    void notify_add_event(agent_t* const in) {
        this->_dim = this->arr_divide(in->learner->Q.size(), this->_grind);
        this->_recall.resize(this->_dim);
        this->refmat.resize(this->_dim);
        this->_recall = 0;
    }
    /**
     * @brief notify_enter_event on every learning cycle start we reset the ref counters
     */
    void notify_enter_event(agent_t* const)
    { this->refmat = 0; }
    /**
     * @brief notify_on_event on every move update the ref counter
     */
    void notify_on_event(agent_t* const in)
    { this->refmat(this->arr_divide(in->_current_state, this->_grind))++; }
    /**
     * @brief notify_deinit_event after each cycle, recall or update the recall memory
     */
    void notify_deinit_event(agent_t* const) {
        size_t k = 0;
        this->refmat
            .for_each(
                [&k, this](auto& j) {
                    if(j == 0)
                        j = this->_recall.data()[k++];
                    else
                        this->_recall.data()[k++] = j;
                });
        assert(this->refmat.min() == this->_recall.min());
    }
    /**
     * @brief notify_exit_event printing the reference counts at the end of learning cycles
     */
    void notify_exit_event(agent_t* const) {
        return;
        this->refmat.for_each([](const auto& j) { cerr << j << " "; });
        cerr << endl;
    }
};

#endif // PLUGIN_REFMAT_HPP
