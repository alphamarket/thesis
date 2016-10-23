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
    // if the current agent state holds valid?
    if(this->operator ()(this->_pos_agent) == Agent())
        // just remove the agent from the block.
        this->operator ()(this->_pos_agent) -= Agent();
    // otherwise we have lost the agent!
    else
        // search through all block for removing it.
        this->foreach_block([](auto, auto b){ b -= Agent(); return true; });
    // add the agent to the given state
    this->operator ()(s) += Agent();
    // keep track of the agent's position
    this->_pos_agent = s;
    // if there is a refmat defined?
    if(this->ref_size && this->_refmat.size())
        // update the reference value for current stage
        this->current_refmat()[s[0] / this->ref_size][s[1] / this->ref_size] += 1;
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
        os << endl;
    }
    return os;
}

maze& maze::operator = (const maze& m) {
    if(m.width != this->width || m.height != this->height || m.ref_size != this->ref_size)
        throw runtime_error("Cannot assign a maze with different configuration!");
    // copy the vectors
    this->_refmat = m._refmat;
    this->_matrix = m._matrix;
    this->_pos_agent = m._pos_agent;
    return *this;
}

void maze::new_refmat() {
    this->_refmat.push_back(refmat_t());
    // create the reference matrix
    for(size_t i = 0; i < size_t(width / ref_size); i++) {
        this->current_refmat().push_back(vector<scalar>());
        for(size_t j = 0; j < size_t(width / ref_size); j++)
            this->current_refmat().back().push_back(0);
    }
}
