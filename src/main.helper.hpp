#ifndef MAIN_HELPER_HPP
#define MAIN_HELPER_HPP

#include "inc/stdafx.hpp"
#include "inc/worlds/maze.hpp"
#include "inc/rl/maze_qlearning.hpp"

#define WALL_REWARD -1

/**
 * @brief create_maze Creates a maze world
 * @param size The size of the world
 * @param goals The vector of pairs position and reward of goals
 * @param walls The position of walls
 * @return The created and initalized maze
 */
maze create_maze(
        const maze::state& size,
        vector<pair<maze::state, scalar>> goals,
        vector<maze::state> walls);
/**
 * @brief set_agent_random Sets the agent at valid random position
 * @param m The maze to act upon
 */
void set_agent_random(maze& m);
/**
 * @brief action_picker Picks an action with greedy atitude
 * @param mq The Maze QLearning instance
 * @param current_hop The current hop#
 * @param current_iter The current iter#
 * @return The picked action
 */
action greedy_action_picker(const Maze_QLearning& mq, const maze::state &state, size_t current_hop, size_t current_iter);
/**
 * @brief action_picker Picks an action with boltsman method
 * @param mq The Maze QLearning instance
 * @param current_hop The current hop#
 * @param current_iter The current iter#
 * @return The picked action
 */
action boltsman_action_picker(const Maze_QLearning& mq, const maze::state &state, size_t current_hop, size_t current_iter);
/**
 * @brief action_handler The action handler
 * @param mq The Maze QLearning instance
 * @param state The current state
 * @param action The action to be taken
 * @return The transited state
 */
maze::state action_handler(const Maze_QLearning& mq, const maze::state& state, action action);
/**
 * @brief iteration_init_callback The QLearning iteration callback
 * @param m The maze world
 * @param iter The iteration it's on
 * @return The configuration for this iteration
 */
QLearningOptions iteration_init_callback(maze& m, size_t iter);

void print_policy(const maze& m, const vector<vector<action>>& policy);

#endif // MAIN_HELPER_HPP
