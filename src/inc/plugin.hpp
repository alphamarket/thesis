#ifndef IPLUGIN_HPP
#define IPLUGIN_HPP

#include "stdafx.hpp"

template<size_t state_dim, size_t action_dim>
class agent;

template<size_t T, size_t S>
class plugin
{
public:
    virtual ~plugin() { }

    virtual std::string name() const = 0;

    virtual void notify_on_event(agent<T, S>* const) { }

    virtual void notify_pre_event(agent<T, S>* const) { }

    virtual void notify_init_event(agent<T, S>* const) { }

    virtual void notify_post_event(agent<T, S>* const) { }

    virtual void notify_exit_event(agent<T, S>* const) { }

    virtual void notify_enter_event(agent<T, S>* const) { }

    virtual void notify_deinit_event(agent<T, S>* const) { }

    virtual void notify_add_event(agent<T, S>* const) { }

    virtual void notify_remove_event(agent<T, S>* const) { }
};

#endif // IPLUGIN_HPP
