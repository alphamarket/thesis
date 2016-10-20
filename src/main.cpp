#include "main.helper.hpp"
#include <future>
#include <chrono>
#include <algorithm>

#include "inc/qcom.hpp"

#define MULTI_AGENT_COUNT       1
#define AGENT_LEARNING_CYCLES   5
#define TRIAL_MAX               200

#define for_each_agent(tid) for(auto tid = 0; tid < MULTI_AGENT_COUNT; tid++)
#define for_each_trial(_try) for(auto _try = 0; _try < TRIAL_MAX; _try++)

/**
 * @brief execute_agent Executes an agent
 * @param m The maze instance clone
 * @param action_list The actions list
 * @param qtable The pre-inited Q-Table
 * @param iteration_max The iteration maximum
 * @param thread_id The agent's thread id
 * @return
 */
QLearningResult execute_agent(maze m, const vector<action> &action_list, const qtable_t& qtable, size_t iteration_max, size_t thread_id);
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
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {
    bool exiting = false;
    vector<future<void>> thread_pool;
    thread_pool.push_back(std::async(std::launch::async, [&exiting]() { while(!exiting) { updateseed(); std::this_thread::sleep_for(std::chrono::milliseconds(100));}} ));

    maze m = create_maze(
                // the size of maze
                {6, 6},
                // define the goals positions and theirs rewards(since they can be variable!)
                {
                    {{1, 5}, +10},
                    {{3, 5}, +10},
                    {{5, 1}, +10}
                },
                // define the walls positions
                {
                    {1, 0}, {1, 1}, {1, 2},
                    {2, 4}, {2, 5},
                    {4, 2}, {4, 3}, {4, 4}
                });

    future<QLearningResult> threads[MULTI_AGENT_COUNT];

    vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};
    auto qtable = Maze_QLearning::init_Qtable(m.width, m.height, action_list);

    vector<scalar> avg_moves;

    for_each_trial(_try) {
        // the result container
        vector<QLearningResult> results;
        // make them learn
        for_each_agent(gid)
            threads[gid] =
                std::async(std::launch::async,
                           execute_agent,
                           m.clone(),
                           action_list,
                           qtable,
                           AGENT_LEARNING_CYCLES,
                           size_t(gid));
        // fetch the results
        for_each_agent(gid)
            results.push_back(threads[gid].get());
        // calc the average moves
        auto avm = get_avg_hop_single(results);
        // store the calced avg moves for later outputing
        avg_moves.insert(avg_moves.end(), avm.begin(), avm.end());
        // if using single agent
        if(MULTI_AGENT_COUNT == 1)
            // don't combine for single agent
            qtable = results.back()._qTable;
        else
            // TODO: combine the intel
            qtable = QCom(results).combine();
        // print progress rate;
        printf("\rProgress rate: %.2f%%", scalar(_try) / TRIAL_MAX * 100);
    }

    cout <<"\r";

    for(size_t i = 0; i < avg_moves.size(); i++)
        cout << accumulate(avg_moves.begin(), avg_moves.begin() + i + 1, 0.0) / (i + 1) << " ";

    exiting = true;
    cout << endl << "Waiting for " << thread_pool.size() << " threads to finish!";
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    exit(0);
}

QLearningResult execute_agent(maze m, const vector<action>& action_list, const qtable_t& qtable, size_t iteration_max, size_t __unused thread_id) {
    set_agent_random(m);
    Maze_QLearning mq(qtable, m, action_list);
    QLearningResult result = mq.execute(action_picker, action_handler, iteration_init_callback, iteration_max);
    return result;
}

vector<scalar> get_avg_hop_single(const vector<QLearningResult>& results) {
    vector<scalar> out(results[0]._hops.size(), 0.);
    for_each_agent(tid) {
        assert(out.size() == results[tid]._hops.size());
        for(size_t i = 0; i < results[tid]._hops.size(); i++)
            out[i] += results[tid]._hops[i] / MULTI_AGENT_COUNT;
    }
    return out;
}

vector<scalar> get_avg_hop_clustered(const vector<QLearningResult> &results) {
    vector<scalar> out = { 0 };
    for_each_agent(tid) {
        auto hop = results[tid]._hops;
        out.back() += (accumulate(hop.begin(), hop.end(), 0.0) / (hop.size() * MULTI_AGENT_COUNT));
    }
    return out;
}
