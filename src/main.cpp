#include "main.helper.hpp"
#include <future>
#include <chrono>
#include <algorithm>

#include "inc/qcom.hpp"

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

    vector<maze> worlds;
    vector<scalar> avg_moves;
    vector<QLearningResult> results;
    vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};
    auto qtable = Maze_QLearning::init_Qtable(m.width, m.height, action_list);

	// clone the world for each agent
    foreach_agent(gid) worlds.push_back(m.clone());

    foreach_trial(_try) {
        // the result container
        results.clear();
        // make them learn
        foreach_agent(gid)
            threads[gid] =
                std::async(std::launch::async,
                           execute_agent,
                           worlds[gid],
                           action_list,
                           qtable,
                           greedy_action_picker,
                           Q_updator,
                           AGENT_LEARNING_CYCLES,
                           size_t(gid));
        // fetch the results
        foreach_agent(gid)
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

    assert(worlds.size() == results.size());

    foreach_agent(gid) {
        cout << endl << endl << "POLICY FOR AGENT# " << gid << endl << endl;
        print_policy(worlds[gid], results[gid]._policy);
    }

    exiting = true;
    cout << endl << "Waiting for " << thread_pool.size() << " threads to finish!";
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    exit(0);
}
