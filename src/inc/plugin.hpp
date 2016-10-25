#ifndef IPLUGIN_HPP
#define IPLUGIN_HPP

class plugin
{
public:
    virtual void notify_on_event(void* const) { }

    virtual void notify_pre_event(void* const) { }

    virtual void notify_init_event(void* const) { }

    virtual void notify_post_event(void* const) { }

    virtual void notify_exit_event(void* const) { }

    virtual void notify_enter_event(void* const) { }

    virtual void notify_deinit_event(void* const) { }
};

#endif // IPLUGIN_HPP
