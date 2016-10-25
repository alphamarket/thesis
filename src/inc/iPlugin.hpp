#ifndef IPLUGIN_HPP
#define IPLUGIN_HPP

class IPlugin
{
public:
    virtual void notify_on_event(void* const) = 0;

    virtual void notify_pre_event(void* const) = 0;

    virtual void notify_init_event(void* const) = 0;

    virtual void notify_after_event(void* const) = 0;

    virtual void notify_deinit_event(void* const) = 0;
};

#endif // IPLUGIN_HPP
