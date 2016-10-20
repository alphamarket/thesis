#include "rl/maze_qlearning.hpp"

Maze_QLearning::Maze_QLearning(maze &m, const vector<action>& action_list)
    : _Q(this->init_Qtable(m.width, m.height, action_list)), _m(&m), _actions_list(action_list)
{
    if(!action_list.size())
        throw runtime_error("Expecting some action list, but got none!");
}


Maze_QLearning::Maze_QLearning(const qtable_t& q, maze& m, const vector<action>& action_list)
    : _Q(q), _m(&m), _actions_list(action_list)
{ }

Maze_QLearning::~Maze_QLearning()
{ }

QLearningResult Maze_QLearning::execute(
        action_func action_picker,
        actor_func actor_handler,
        function<QLearningOptions(maze&, size_t)> iteration_init_callback,
        size_t iteration_max) {
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
        opts.push_back(iteration_init_callback(*this->_m, iteration));
        // while not reached to final state (the termination condition)
        while(this->_m->agent_block().type() != block::GOAL) {
            // pick an action
            action a = action_picker(*this, this->_m->agent_location(), hops.back()++, (iteration_max - iteration));
            // calc. the transition that the action causes
            maze::state sprim = actor_handler(*this, this->_m->agent_location(), a);
            // get the reward of the next state
            scalar reward = (*this->_m)(sprim).reward();
            // \max_{a'} Q[s',a']
            scalar qprim = -INFINITY;
            // search over every action for find the maximum impact in next state
            for(auto act : this->_actions_list) qprim = max(qprim, this->Q(sprim, act));
            // update the Q table for current state that agent still
            this->Q(this->_m->agent_location(), a) =
                    (1 - opts.back().alpha) *
                            // Q[s,a]
                            this->Q(this->_m->agent_location(), a) +
                    opts.back().alpha *
                            // r + \gamma * \max_{a'} Q[s',a']
                            (reward +opts.back().gamma * qprim);
            // apply the actual transition
            this->_m->agent_location(sprim);
        }
    }
    // fail-check
    assert(hops.size() == iteration_max);
    return { hops, opts, iteration_max, this->_Q, this->get_policy(), this->_m };
}

policy_t Maze_QLearning::get_policy() const {
    policy_t policy;
    for(size_t i = 0; i < this->_m->width; i++) {
        policy.push_back(vector<action>());
        for(size_t j = 0; j < this->_m->height; j++) {
            policy[i].push_back(0);
            scalar qprim = -INFINITY;
            maze::state state = {i,j};
            // search over every action for find the maximum impact in next state
            for(auto act : this->actions_list()) { if(qprim < this->Q(state, act)) { qprim = this->Q(state, act); policy[i].back() = act; } }
        }
    }
    return policy;
}
