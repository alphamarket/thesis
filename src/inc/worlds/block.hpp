#pragma once
#include <iomanip>

#include "inc/stdafx.hpp"
#include "colormod.hpp"

class Agent { };

class block {
public:
    enum type {
        NONE = 0, EMPTY = 1, WALL = 2, GOAL = 3
    };
protected:
    /**
     * @brief _type The type of block
     */
    type _type;
    /**
     * @brief _reward The reward associated to the block
     */
    scalar _reward;
    /**
     * @brief _has_agent Check if the block contains an agent or not
     */
    bool _has_agent;
public:
    /**
     * @brief block The ctor
     * @param t The type of the block
     * @param r The associated reward of the block
     */
    block(type t = NONE, scalar r = 0)
        : _type(t), _reward(r), _has_agent(0)
    { }
    /**
     * @brief block Copy ctor
     * @param b The block to be copied from
     */
    block(const block& b)
    { this->_type = b._type; this->_reward = b._reward; this->_has_agent = b._has_agent; }
    /**
     * @brief operator = The assignment operator
     * @param b The assigned block
     * @return The current instance
     */
    block& operator =(const block& b)
    { this->_type = b._type; this->_reward = b._reward; this->_has_agent = b._has_agent; return *this; }
    /**
     * @brief block The move ctor
     * @param b The block to be moved from
     */
    block(block&& b) noexcept
        : _type(std::move(b._type)),
          _reward(std::move(b._reward)),
          _has_agent(std::move(b._has_agent))
    { }
    /**
     * @brief operator += Addes an agent to current block
     */
    inline void operator += (Agent) { this->_has_agent = 1; }
    /**
     * @brief operator -= Removes the agent to current block
     */
    inline void operator -= (Agent) { this->_has_agent = 0; }
    /**
     * @brief operator == Checks if current block contains an agent or not
     */
    inline bool operator == (Agent) const { return this->_has_agent; }
    /**
     * @brief operator == Checks if current block does not contains an agent or not
     */
    inline bool operator != (Agent) const { return !this->_has_agent; }
    /**
     * @brief type Get the type of the block
     */
    inline type type() const { return this->_type; }
    /**
     * @brief reward Get the reward associated to current instance
     */
    inline scalar reward() const { return this->_reward; }
    /**
     * @brief reward Set the reward associated to current instance
     */
    inline scalar reward(scalar val) { return this->_reward = val; }
    /**
     * @brief type_tostring Get the current instance type as an string
     */
    inline string type_tostring() const {
        string s[] = { "NONE", "EMPTY", "WALL", "GOAL" };
        return s[int(this->_type)];
    }
    /**
     * @brief operator << The out stream operator
     * @param os Stream object
     * @param b The block to out streamize
     * @return The stream object
     */
    friend ostream& operator<<(ostream& os, const block& b) {
        stringstream ss;
        ss << std::setprecision(1) << std::fixed << ((b == Agent()) ? "AGENT" : b.type_tostring()) << "["<<b._reward<<"]";
        int ssize = abs(13 - ss.str().length());
        ss.str("");
        ss << std::setprecision(1) << std::fixed;
        if(b == Agent())
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
