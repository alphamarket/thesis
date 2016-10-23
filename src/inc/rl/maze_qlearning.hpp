#ifndef QLEARNING_H
#define QLEARNING_H

#include <functional>

#include "stdafx.hpp"
#include "qlearning.hpp"
#include "worlds/maze.hpp"

class Maze_QLearning
{
public:
    /**
     * @brief action_func The action picker interface
     * @param Maze_QLearning The learner handle
     * @param state The current state
     * @param current_hop The current hop while trying to reach to goal
     * @param current_iteration The current iteration of learning
     * @return The action
     */
    typedef function<action(const Maze_QLearning&, const maze::state&, size_t, size_t)> action_func_t;
    /**
     * @brief actor_func The actor handler to caculate the transition on a state based on an action
     * @param Maze_QLearning The learner handle
     * @param state The current state to apply the action up
     * @param action The action to be taken
     * @return The result state
     */
    typedef function<maze::state(const Maze_QLearning&, const maze::state&, action)> actor_func_t;
    /**
     * @brief qupdate_func Calcs the update value for Q[s,a]
     * @param opt The Qlearning option instance
     * @param qsa The current state/action Q value(Q[s,a])
     * @param qsa_prim The next state/action Q value(Q[s',a'])
     * @param reward The obtained reward value by [s -> s'] transition
     * @param hop The current hop
     * @return scalar The new Q value for Q[s,a]
     */
    typedef function<scalar(const QLearningOptions&, const scalar&, const scalar&, const scalar&, const size_t&)> qupdate_func_t;
    typedef function<QLearningOptions(maze &, size_t)> iteration_init_func_t;
protected:
    qtable_t _Q;
    maze* const _m;
    vector<action> _actions_list;
public:
    Maze_QLearning(maze& m, const vector<action>& action_list);
    Maze_QLearning(const qtable_t& q, maze& m, const vector<action>& action_list);
    ~Maze_QLearning();

    policy_t get_policy() const;
    QLearningResult execute(action_func_t action_picker, actor_func_t actor_handler, qupdate_func_t, iteration_init_func_t iteration_init_callback, size_t iteration_max = 1);

    inline maze& get_maze() { return *this->_m; }
    inline maze get_maze() const { return *this->_m; }
    inline vector<action> actions_list() const { return this->_actions_list; }
    inline scalar& Q(const maze::state& s, const uint& action) { return this->_Q[s[0]][s[1]][action]; }
    inline scalar Q(const maze::state& s, const uint& action) const { return this->_Q[s[0]][s[1]][action]; }
    inline qtable_t& QTable() { return this->_Q; }
    inline qtable_t QTable() const { return this->_Q; }

    static qtable_t init_Qtable(const size_t width, const size_t height, const vector<action>& action_list) {
        auto q = vector<vector<vector<scalar>>>();
        for(size_t i = 0; i < width; i++) {
            q.push_back(vector<vector<scalar>>());
            for(size_t j = 0; j < height; j++) {
                q[i].push_back(vector<scalar>());
                for(size_t k = 0; k < action_list.size(); k++) {
                    q[i][j].push_back(0);
                }
            }
        }
        return q;
     }
};

#endif // QLEARNING_H
