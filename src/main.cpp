#include "main.helper.hpp"
#include <future>

#define MULTI_AGENT_COUNT 3
#define AGENT_LEARNING_CYCLES   10
#define TRIAL_MAX               1000

#define for_each_agent(tid) for(auto tid = 0; tid < MULTI_AGENT_COUNT; tid++)
#define for_each_trial(_try) for(auto _try = 0; _try < TRIAL_MAX; _try++)

QLearningResult execute_agent(maze m, const vector<action> &action_list, const qtable_t& qtable, size_t iteration_max, size_t thread_id);

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {

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


    QLearningResult results[MULTI_AGENT_COUNT];
    future<QLearningResult> threads[MULTI_AGENT_COUNT];

    vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};
    auto qtable = Maze_QLearning::init_Qtable(m.width, m.height, action_list);

    for_each_trial(_try) {
        // make them learn
        for_each_agent(tid)
            threads[tid] =
                std::async(std::launch::async,
                           execute_agent,
                           m.clone(),
                           action_list,
                           qtable,
                           AGENT_LEARNING_CYCLES,
                           size_t(tid));
        // fetch the results
        for_each_agent(tid)
            results[tid] = threads[tid].get();
        // TODO: combine the intel
        cout << "\r" << scalar(_try) / TRIAL_MAX;
    }

	return 0;
}

QLearningResult execute_agent(maze m, const vector<action>& action_list, const qtable_t& qtable, size_t iteration_max, size_t __unused thread_id) {
    set_agent_random(m);
    Maze_QLearning mq(qtable, m, action_list);
    QLearningResult result = mq.execute(action_picker, action_handler, iteration_init_callback, iteration_max);
    return result;
}
