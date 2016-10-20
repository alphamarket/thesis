#pragma once
#include <iomanip>

#include "inc/stdafx.hpp"
#include "colormod.hpp"

class block {
public:
    enum type {
        NONE = 0, EMPTY = 1, WALL = 2, GOAL = 3
    };
protected:
    type _type;
    scalar _reward;
    bool _has_agent;
public:

    block(type t = NONE, scalar r = 0)
        : _type(t), _reward(r), _has_agent(0)
    { }

    block(const block& b)
    { this->_type = b._type; this->_reward = b._reward; this->_has_agent = b._has_agent; }

    block& operator =(const block& b)
    { this->_type = b._type; this->_reward = b._reward; this->_has_agent = b._has_agent; return *this; }

    block(block&& b) noexcept
        : _type(std::move(b._type)),
          _reward(std::move(b._reward)),
          _has_agent(std::move(b._has_agent))
    { }

    inline void add_agent() { this->_has_agent = 1; }
    inline void remove_agent() { this->_has_agent = 0; }
    inline type get_type() const { return this->_type; }
    inline scalar get_reward() const { return this->_reward; }
    inline bool has_agent() const { return this->_has_agent; }
    inline scalar set_reward(scalar val) { return this->_reward = val; }

    inline string type_tostring() const {
        string s[] = { "NONE", "EMPTY", "WALL", "GOAL" };
        return s[int(this->_type)];
    }

    friend ostream& operator<<(ostream& os, const block& b) {
        stringstream ss;
        ss << std::setprecision(1) << std::fixed << (b.has_agent() ? "AGENT" : b.type_tostring()) << "["<<b._reward<<"]";
        int ssize = abs(13 - ss.str().length());
        ss.str("");
        ss << std::setprecision(1) << std::fixed;
        if(b.has_agent())
            ss << Color::Modifier(Color::FG_GREEN) << "AGENT";
        else if(b._type == block::WALL)
            ss << Color::Modifier(Color::FG_RED) << "WALL";
        else if(b._type == block::GOAL)
            ss << Color::Modifier(Color::FG_YELLOW) << "GOAL";
        else
            ss << b.type_tostring();
        ss << Color::Modifier(Color::FG_DEFAULT);
        ss << "["<<b._reward<<"]";
        while(ssize--) ss << " ";
        os << ss.str();
        return os;
    }

};
