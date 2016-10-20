#include "rl/maze_qlearning.hpp"

Maze_QLearning::Maze_QLearning(maze &m, const vector<action>& action_list)
    : _m(m), _actions_list(action_list)
{
    if(!action_list.size())
        throw runtime_error("Expecting some action list, but got none!");
    this->_Q = new scalar**[m.width];
    for(size_t i = 0; i < m.width; i++) {
        this->_Q[i] = new scalar*[m.height];
        for(size_t j = 0; j < m.height; j++) {
            this->_Q[i][j] = new scalar[action_list.size()]();
        }
    }
}

Maze_QLearning::~Maze_QLearning() {
    for(size_t i = 0; i < this->_m.width; i++){
        for(size_t j = 0; j < this->_m.height; j++)
            delete[] this->_Q[i][j];
        delete[] this->_Q[i];
    }
    delete[] this->_Q;
}

QLearningResult Maze_QLearning::execute(
        action_func action_picker,
        actor_func actor_handler,
        size_t iteration_max,
        function<QLearningOptions(maze&, size_t)> iteration_init_callback) {
    // total hop done in this execution
    vector<size_t> hops;
    // the options obtained in this execution
    vector<QLearningOptions> opts;
    // the iteration
    size_t iteration = iteration_max;
    // for each iteration defined
    while(iteration--) {
        // init the hop count for this iteration to zero
        hops.push_back(0);
        // init callback at each iteration for any pre-processing phase
        // such as relocating the agent or apply changes in environment
        opts.push_back(iteration_init_callback(this->_m, iteration));
        // while not reached to final state (the termination condition)
        while(this->_m.agent_block().type() != block::GOAL) {
            // pick an action
            action a = action_picker(*this, this->_m.agent_location(), hops.back()++, (iteration_max - iteration));
            // calc. the transition that the action causes
            maze::state sprim = actor_handler(*this, this->_m.agent_location(), a);
            // get the reward of the next state
            scalar reward = this->_m(sprim).reward();
            // \max_{a'} Q[s',a']
            scalar qprim = -INFINITY;
            // search over every action for find the maximum impact in next state
            for(auto act : this->_actions_list) qprim = max(qprim, this->Q(sprim, act));
            // update the Q table for current state that agent still
            this->Q(this->_m.agent_location(), a) =
                    (1 - opts.back().alpha) *
                            // Q[s,a]
                            this->Q(this->_m.agent_location(), a) +
                    opts.back().alpha *
                            // r + \gamma * \max_{a'} Q[s',a']
                            (reward +opts.back().gamma * qprim);
            // apply the actual transition
            this->_m.agent_location(sprim);
        }
    }
    // fail-check
    assert(hops.size() == iteration_max);
    return { hops, opts, iteration_max };
}
