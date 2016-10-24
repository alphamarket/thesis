#ifndef MAIN_HELPER_HPP
#define MAIN_HELPER_HPP

#include "inc/stdafx.hpp"
#include "inc/worlds/maze.hpp"
#include "inc/rl/maze_qlearning.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

/**
 * @brief create_maze Creates a maze world
 * @param size The size of the world
 * @param goals The vector of pairs position and reward of goals
 * @param walls The position of walls
 * @return The created and initalized maze
 */
maze create_maze(const maze::state& size, const size_t ref_size, vector<pair<maze::state, scalar>> goals, vector<maze::state> walls);
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
 * @brief action_picker Picks an action with boltzmann method
 * @param mq The Maze QLearning instance
 * @param current_hop The current hop#
 * @param current_iter The current iter#
 * @return The picked action
 */
action boltzmann_action_picker(const Maze_QLearning& mq, const maze::state &state, size_t current_hop, size_t current_iter);
/**
 * @brief action_handler The action handler
 * @param mq The Maze QLearning instance
 * @param state The current state
 * @param action The action to be taken
 * @return The transited state
 */
maze::state action_handler(const maze &mq, const maze::state& state, action action);
/**
 * @brief iteration_init_callback The QLearning iteration callback
 * @param m The maze world
 * @param iter The iteration it's on
 * @return The configuration for this iteration
 */
QLearningOptions iteration_init_callback(maze& m, size_t iter);
/**
 * @brief print_policy Prints the learned policy of given maze
 * @param m The maze that RL learned on it
 * @param policy The extracted policy
 */
void print_policy(const maze& m, const vector<vector<action>>& policy);
/**
 * @brief execute_agent Executes an agent
 * @param m The maze instance clone
 * @param action_list The actions list
 * @param qtable The pre-inited Q-Table
 * @param iteration_max The iteration maximum
 * @param sep The SEP matrix
 * @param shock The SHOCK matrix
 * @return The execution result
 */
QLearningResult execute_agent(
        Maze_QLearning mq,
        Maze_QLearning::action_func_t action_picker,
        Maze_QLearning::qupdate_func_t Q_updator,
        size_t iteration_max);
/**
 * @brief get_avg_hop_single Calculates the avg hop of EVERY hop for all agents
 * @param results The agent's qlearning results
 * @return The avg hop of vector
 */
vector<scalar> get_avg_hop_single(const vector<QLearningResult>& results);
/**
 * @brief get_avg_hop_clustered Calculates the avg hop of ALL hops for all agents
 * @param results The agent's qlearning results
 * @return The avg hop of vector (a single value)
 */
vector<scalar> get_avg_hop_clustered(const vector<QLearningResult>& results);
/**
 * @brief greedy_qupdator The greedy Q Updator
 * @param opt The options
 * @param qsa The current state/action Q[s,a] value
 * @param qsa_prim The next state/action Q[s',a'] value
 * @param reward The reward acquired
 * @param hop The curren hop
 * @return The new Q[s,a] update value
 */
scalar Q_updator(const QLearningOptions& opt, const scalar& qsa, const scalar& qsa_prim, const scalar& reward, const size_t& hop);
/**
 * @brief process_args Processes the program options
 * @param argc The argc
 * @param argv The argv
 * @return The varible map of options
 */
po::variables_map process_args(int argc, char** argv);
/**
 * @brief print_refmat Prints the refmat
 * @param refmat The refmats
 */
void print_refmat(const vector<vector<vector<scalar>>>& refmat);
/**
 * @brief recall_refmat Recalls the refmats from past if necessary
 * @param now The current refmats
 * @param past The pass refmat
 */
void recall_refmat(vector<maze::refmat_t>& now, maze::refmat_t& past);
/**
 * @brief test_sep Prints out the tests for SEP
 */
void test_sep();

#endif // MAIN_HELPER_HPP
