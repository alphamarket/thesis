#include "inc/stdafx.hpp"
#include "inc/agent.hpp"
#include "inc/world.maze.hpp"
#include "inc/unit.tests.hpp"
#include "inc/main.plugins.hpp"
#include "inc/learner.maze.hpp"

template<size_t ITERS, size_t AGENTS, size_t TRIALS, size_t CYCLES>
void execute_over_maze(array<array<array<scalar, TRIALS * CYCLES>, AGENTS>, ITERS>& data, const size_t& CURRENT_ITER) {
    vector<maze> worlds;
    vector<agent<2, 1>> agents;
    vector<learner_maze> learners;
    vector<vector<plugin<2, 1>*>> plugins;
    for(size_t i = 0; i < AGENTS; i++) {
        worlds.push_back(maze({6, 6}));
        worlds.back().define_values({
            // define a goal
            {{1, 5}, block(10, 1, 1)},
            {{3, 5}, block(10, 1, 1)},
            {{5, 0}, block(10, 1, 1)},
            // define walls
            {{1, 0}, block(-1, 0, 0)},
            {{1, 1}, block(-1, 0, 0)},
            {{1, 2}, block(-1, 0, 0)},
            {{2, 4}, block(-1, 0, 0)},
            {{2, 5}, block(-1, 0, 0)},
            {{4, 2}, block(-1, 0, 0)},
            {{4, 3}, block(-1, 0, 0)},
            {{4, 4}, block(-1, 0, 0)},
        });
        learners.push_back(learner_maze({worlds.back().size()[0], worlds.back().size()[1], worlds.back().action_no()}));
        plugins.push_back({
            { new plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>() },
            { new plugin_reset_agent<maze::STATE_DIM, maze::ACTION_DIM>() },
            { new plugin_SEP<maze::STATE_DIM, maze::ACTION_DIM>() }
        });
    }
    // contains previous hops
    array<size_t, AGENTS> hop_sures;
    for(size_t i = 0; i < AGENTS; i++) {
        hop_sures[i] = 0;
        agents.push_back(agent<2, 1>(&worlds[i], &learners[i]));
        foreach_elem(_plugin, plugins[i]) agents.back() += _plugin;
    }
    for(size_t trial = 0; trial < TRIALS; trial++) {
        // execute
        for(size_t i = 0; i < AGENTS; i++) agents[i].execute(CYCLES, AGENTS > 1);
        // wait agent to merge
        for(size_t i = 0; i < AGENTS; i++) {
            agents[i].wait_to_execute();
            // check things went as expected
            assert((agents[i].get_plugin<plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>>(plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>().name())->hops.size() - hop_sures[i]) == CYCLES);
            // if az expected? update it
            hop_sures[i] = agents[i].get_plugin<plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>>(plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>().name())->hops.size();
        }
        // here we do the combination
    }
    // export the collected data
    for(size_t i = 0; i < AGENTS; i++) {
        auto h = agents[i].get_plugin<plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>>(plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>().name());
        assert(h->hops.size() == TRIALS * CYCLES);
        // store the data
        std::copy(h->hops.begin(), h->hops.end(), data[CURRENT_ITER][i].begin());
        // release all plugin's resources
        foreach_elem(_plugin, plugins[i]) delete _plugin;
    }
}

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {
    // exiting flag
    bool exiting = false;
    vector<future<void>> thread_pool;
    // random seed updator
    thread_pool.push_back(std::async(std::launch::async, [&exiting]() { while(!exiting) { updateseed(); std::this_thread::sleep_for(std::chrono::milliseconds(100));}} ));

    // execute the pre-defined tests
    execute_tests();

    const size_t ITERS = 20;
    const size_t CYCLES = 5;
    const size_t TRIALS = 200;
    const size_t AGENT_COUNT = 3;

    // ITERS x AGENTS x [TRIALS * CYCLES]
    array<array<array<scalar, TRIALS * CYCLES>, AGENT_COUNT>, ITERS> data;

    // execute for each iteration and collect the data
    for(size_t iter = 0; iter < ITERS; iter++)
        execute_over_maze<ITERS, AGENT_COUNT, TRIALS, CYCLES>(data, iter);

    // try to average data over AGENTS and ITERs
    array<scalar, TRIALS * CYCLES> avg_data;
    for(size_t k = 0; k < TRIALS * CYCLES; k++) {
        for(size_t i = 0; i < ITERS; i++) {
            for(size_t j = 0; j < AGENT_COUNT; j++) {
                avg_data[k] += data[i][j][k] / (AGENT_COUNT * ITERS);
            }
        }
    }

    // print accumulate the sums of avg data
    for(size_t i = 0; i < avg_data.size(); i++)
        cout << accumulate(avg_data.begin(), avg_data.begin() + i + 1, 0.0) / (i + 1) << " ";

    // exit the program
    exiting = true;
    while(!thread_pool.empty()) { thread_pool.back().get(); thread_pool.pop_back(); }
    exit(EXIT_SUCCESS);
}
