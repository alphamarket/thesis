#ifndef QLEARNING_H
#define QLEARNING_H

#include <functional>

#include "stdafx.hpp"
#include "qlearning.hpp"
#include "worlds/maze.hpp"

class Maze_QLearning
{
public:
    typedef uint action;
    /**
     * @brief action_func The action picker interface
     * @param Maze_QLearning The learner handle
     * @param state The current state
     * @param current_hop The current hop while trying to reach to goal
     * @param current_iteration The current iteration of learning
     * @return The action
     */
    typedef function<action(const Maze_QLearning&, const maze::state&, size_t, size_t)> action_func;
    /**
     * @brief actor_func The actor handler to caculate the transition on a state based on an action
     * @param Maze_QLearning The learner handle
     * @param state The current state to apply the action up
     * @param action The action to be taken
     * @return The result state
     */
    typedef function<maze::state(const Maze_QLearning&, const maze::state&, action)> actor_func;
protected:
    maze* const _m;
    scalar*** _Q;
    vector<action> _actions_list;
public:
    Maze_QLearning(maze& m, const vector<action>& action_list);
    ~Maze_QLearning();

    action** get_policy() const;
    QLearningResult execute(action_func action_picker, actor_func actor_handler, function<QLearningOptions(maze &, size_t)> iteration_init_callback, size_t iteration_max = 1);

    inline maze& get_maze() { return *this->_m; }
    inline maze get_maze() const { return *this->_m; }
    inline vector<action> actions_list() const { return this->_actions_list; }
    inline scalar& Q(const maze::state& s, const uint& action) { return this->_Q[s[0]][s[1]][action]; }
    inline scalar Q(const maze::state& s, const uint& action) const { return this->_Q[s[0]][s[1]][action]; }

};

#endif // QLEARNING_H
