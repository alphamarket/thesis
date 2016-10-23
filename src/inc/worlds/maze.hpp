#pragma once

#include <functional>

#include "inc/stdafx.hpp"
#include "block.hpp"

class maze
{
public:
    typedef vector<size_t> state;
    typedef vector<vector<block>> matrix_t;
    typedef vector<vector<scalar>> refmat_t;
protected:
    matrix_t
        /**
         * @brief _matrix The world's matrix
         */
        _matrix;
    vector<refmat_t>
        /**
         * @brief _refmat The world's reference matrix
         */
        _refmat;
public:
    /**
     * @brief _pos_agent The state of agent
     */
    state _pos_agent;

    const size_t
        /**
         * @brief width The width of world
         */
        width,
        /**
         * @brief height The height of world
         */
        height,
        /**
         * @brief ref_size The reference size
         */
        ref_size;
    enum actions { NONE = 0, TOP, RIGHT, DOWN, LEFT };
public:
    /**
     * @brief maze The ctor
     * @param width The width of the world to create
     * @param height The height of world to create
     */
    maze(const size_t width, const size_t height, const size_t ref_size = 0)
        : _pos_agent({0, 0}), width(width), height(height), ref_size(ref_size)
    {
        if(ref_size && (width % ref_size != 0 || height % ref_size != 0))
            throw runtime_error("The width and height should be dividable by `ref_size`!");
        // create the matrix
        for(size_t i = 0; i < width; i++) {
            this->_matrix.push_back(vector<block>());
            for(size_t j = 0; j < height; j++)
                this->_matrix.back().push_back(block(block::EMPTY));
        }
    }
    /**
     * @brief maze The copy ctor
     * @param m The maze to copy from
     */
    maze(const maze& m)
        : width(m.width), height(m.height), ref_size(m.ref_size)
    { this->operator = (m); }
    /**
     * @brief maze The move ctor
     * @param m The maze to get moved from
     */
    maze(maze&& m)
        :  _matrix(move(m._matrix)), _refmat(move(m._refmat)), _pos_agent(move(m._pos_agent)), width(move(m.width)), height(move(m.height)), ref_size(move(m.ref_size))
    { }
    /**
     * @brief ~maze The dtor
     */
    ~maze()
    { }

    /**
     * @brief clone Clones the current instance to a new instance
     * @return The cloned instance
     */
    inline maze clone() const
    { return maze(*this); }
    /**
     * @brief is_valid Checks if the given state fits in current world's configuration
     * @param s The target state
     * @return Returns true of the state is valid in current world; otherwise false.
     */
    inline bool is_valid(const state& s) const
    { return s[0] < this->width && s[1] < this->height; }
    /**
     * @brief ensure_state Ensures that the given state is valid; otherwise a runtime error will raise
     * @param s The target state
     */
    inline void ensure_state(const state& s) const
    { if(!this->is_valid(s)) throw runtime_error("State out of range!"); }
    /**
     * @brief operator () Accesses to the blocks of the world
     * @param s The state to access
     * @return The referenced of the block
     */
    inline block& operator () (const state& s) { this->ensure_state(s); return this->_matrix[s[0]][s[1]]; }
    /**
     * @brief operator () Accesses to the blocks of the world
     * @param s The state to access
     * @return A copy of the block
     */
    inline block operator () (const state& s) const { this->ensure_state(s); return this->_matrix[s[0]][s[1]]; }
    /**
     * @brief action_tostring Converts actions to string
     * @param a The action
     * @return The strifigy action
     */
    inline string action_tostring(actions a) const { string s[] = { "NONE", "UP", "RIGHT", "DOWN", "LEFT" }; return s[int(a)]; }
    /**
     * @brief agent_location Sets the agent's state in the world
     * @param s The desired state
     */
    bool agent_location(const state& s);
    /**
     * @brief agent_location Get the agent's location in the world
     * @return The agent's state
     */
    state agent_location() const { return this->_pos_agent; }
    /**
     * @brief agent_block Get the agent's block in the world
     * @return The agent's block
     */
    block agent_block() const { return this->operator()(this->agent_location()); }
    /**
     * @brief foreach_block Foreach block iterator
     * @param callback The callback for each state and block
     */
    void foreach_block(std::function<bool(state, block&)> callback);
    /**
     * @brief operator = The assignment operator
     * @param m The assigned maze
     * @return Current instance
     */
    maze& operator = (const maze& m);
    /**
     * @brief matrix get a copy version of maze's matrix
     */
    matrix_t matrix() const { return this->_matrix; }
    /**
     * @brief matrix get the referenece matrix
     */
    vector<refmat_t>& refmat() { return this->_refmat; }
    /**
     * @brief matrix get a copy version of maze's referenece matrix
     */
    vector<refmat_t> refmat() const{ return this->_refmat; }
    /**
     * @brief new_refmat creates new refmat
     */
    void new_refmat();
    /**
     * @brief current_refmat get current refmat
     */
    inline refmat_t& current_refmat() { return this->_refmat.back(); }
    /**
     * @brief current_refmat get a copy of current refmat
     */
    inline refmat_t current_refmat() const { return this->_refmat.back(); }
    /**
     * @brief reset_refmat resets the reference matrix
     */
    inline void reset_refmat() { foreach_elem(&k, this->_refmat) foreach_elem(&e, k) foreach_elem(&k, e) k = 0; }
    /**
     * @brief operator << The out stream operator
     * @param os Stream object
     * @param m The maze to out streamize
     * @return The stream object
     */
    friend ostream& operator<<(ostream& os, const maze& m);
};
