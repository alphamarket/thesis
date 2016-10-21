#include "main.helper.hpp"
#include <future>
#include <chrono>
#include <algorithm>

#include "inc/qcom.hpp"

void print_refmat(const vector<vector<block>>& refmat) {
    foreach_elem(e, refmat) {
        foreach_elem(k, e)
            cout << k.value() << " ";
        cout <<endl;
    }
}

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int argc, char** argv) {
    bool exiting = false;
    vector<future<void>> thread_pool;
    thread_pool.push_back(std::async(std::launch::async, [&exiting]() { while(!exiting) { updateseed(); std::this_thread::sleep_for(std::chrono::milliseconds(100));}} ));

    maze m = create_maze(
                // the size of maze
                {6, 6},
                // the reference size
                3,
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
    size_t ____i_max = 1;
    if(argc > 1) ____i_max = stoi(string(argv[1]));
    for(size_t ____i = 0; ____i < ____i_max; ____i++) {

        vector<scalar> avg_moves;
        vector<QLearningResult> results;
        future<QLearningResult> threads[MULTI_AGENT_COUNT];
        vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};
        qtable_t qtable = Maze_QLearning::init_Qtable(m.width, m.height, action_list);

        foreach_trial(_try) {
            // the result container
            results.clear();
            // make them learn
            foreach_agent(gid)
                threads[gid] =
                    std::async(std::launch::async,
                               execute_agent,
                               m,
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
                qtable = results.back()._qtable;
            else
                // combine the intel
                qtable = QCom(results, m.ref_size).combine();
        }


        for(size_t i = 0; i < avg_moves.size(); i++)
            cout << accumulate(avg_moves.begin(), avg_moves.begin() + i + 1, 0.0) / (i + 1) << " ";
        cout <<endl;

        assert(results.size() == MULTI_AGENT_COUNT);

        foreach_agent(gid) {
            break;
            cout << endl << endl << "POLICY FOR AGENT# " << gid << endl << endl;
            print_policy(m, results[gid]._policy);
        }
    }

    exiting = true;
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    exit(0);
}
