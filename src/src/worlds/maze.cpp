#include "inc/worlds/maze.hpp"
#include <iomanip>

void maze::foreach_block(std::function<bool(maze::state, block&)> callback) {
    assert(this->_matrix.size() == this->width);
    for(size_t i = 0; i < width; i++) {
        assert(this->_matrix[i].size() == this->height);
        for(size_t j = 0; j < height; j++)
            if(!callback({i,j}, this->_matrix[i][j]))
                return;
    }
}

bool maze::agent_location(const state &s) {
    this->ensure_state(s);
    // cannot move into walls
    if(this->operator ()(s).type() == block::WALL) return false;
    // if we have valid current agent state?
    if(this->_pos_agent[0] >= 0 && this->_pos_agent[1] >= 0) {
        // if the current agent state holds valid?
        if(this->operator ()(this->_pos_agent) == Agent())
            // just remove the agent from the block.
            this->operator ()(this->_pos_agent) -= Agent();
        // otherwise we have lost the agent!
        else
            // search through all block for removing it.
            this->foreach_block([](auto, auto b){ b -= Agent(); return true; });
    }
    // add the agent to the given state
    this->operator ()(s) += Agent();
    // keep track of the agent's position
    this->_pos_agent = s;
    // the success indicator
    return true;
}

ostream& operator<<(ostream& os, const maze& m) {
    assert(m._matrix.size() == m.width);
    for(size_t i = 0; i < m.width; i++) {
        assert(m._matrix[i].size() == m.height);
        for(size_t j = 0; j < m.height; j++) {
            os << m._matrix[i][j];
        }

        cout << endl;
    }
    return os;
}

maze& maze::operator = (const maze& m) {
    if(m.width != this->width || m.height != this->height)
        throw runtime_error("Cannot assign a maze with different configuration!");
    this->_pos_agent = m._pos_agent;
    for(size_t i = 0; i < m.width; i++) {
        this->_matrix.push_back(vector<block>());
        for(size_t j = 0; j < m.height; j++)
            this->_matrix[i].push_back(m({i, j}));
    }
    return *this;
}
