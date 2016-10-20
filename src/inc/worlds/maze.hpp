#pragma once

#include <functional>

#include "inc/stdafx.hpp"
#include "block.hpp"

class maze
{
    vector<vector<block>> _matrix;
public:
    typedef vec2i state;
    state _pos_agent;
    const size_t width, height;
public:
    maze(const size_t width, const size_t height)
        : _pos_agent(-1, -1), width(width), height(height)
    {
        for(size_t i = 0; i < width; i++) {
            this->_matrix.push_back(vector<block>());
            for(size_t j = 0; j < height; j++)
                this->_matrix[i].push_back(block(block::EMPTY));
        }
    }

    maze(const maze& m)
        : width(m.width), height(m.height)
    { this->operator = (m); }

    maze(maze&& m)
        :  _matrix(move(m._matrix)), _pos_agent(move(m._pos_agent)), width(move(m.width)), height(move(m.height))
    { }

    ~maze()
    { }

    inline maze clone() const
    { return maze(*this); }

    inline bool is_valid(const state& s) const
    { return s[0] >= 0 && s[1] >= 0 && s[0] < this->width && s[1] < this->height; }

    inline void ensure_state(const state& s) const
    { if(!this->is_valid(s)) throw runtime_error("State out of range!"); }

    inline block& operator () (const state& s) { return this->operator ()(s[0], s[1]); }
    inline block operator () (const state& s) const { return this->operator ()(s[0], s[1]); }

    inline block& operator () (const size_t x, const size_t y) { this->ensure_state({x,y}); return this->_matrix[x][y]; }
    inline block operator () (const size_t x, const size_t y) const { this->ensure_state({x,y}); return this->_matrix[x][y]; }

    void add_agent(const state& s);
    void foreach_block(std::function<bool(state, block&)> callback);

    maze& operator = (const maze& m);
    friend ostream& operator<<(ostream& os, const maze& m);
};
