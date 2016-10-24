#ifndef PLUGIN_HPP
#define PLUGIN_HPP

class plugin
{
public:
    template<typename T>
    void notify_on_event(T* const) = 0;

    template<typename T>
    void notify_pre_event(T* const) = 0;

    template<typename T>
    void notify_init_event(T* const) = 0;

    template<typename T>
    void notify_after_event(T* const) = 0;

    template<typename T>
    void notify_deinit_event(T* const) = 0;
};

#endif // PLUGIN_HPP
