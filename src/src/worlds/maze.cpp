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

void maze::add_agent(const state &s) {
    this->ensure_state(s);
    if(this->_pos_agent[0] >= 0 && this->_pos_agent[1] >= 0)
        this->operator ()(this->_pos_agent).remove_agent();
    this->operator ()(s).add_agent();
    this->_pos_agent = s;
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
            this->_matrix[i].push_back(m(i, j));
    }
    return *this;
}
