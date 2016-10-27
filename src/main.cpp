#include "inc/stdafx.hpp"
#include "inc/unit.tests.hpp"
#include "inc/main.extend.hpp"
#include "inc/execute.agent.maze.hpp"

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int argc, char** argv) {
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
    const size_t AGENT_COUNT = 1;

    auto opt = process_args(argc, argv);

    fprintf(stderr, "Running %zu agents with %zu iterations %zu times....\n", AGENT_COUNT, TRIALS * CYCLES, ITERS);

    // ITERS x AGENTS x [TRIALS * CYCLES]
    array<array<array<scalar, TRIALS * CYCLES>, AGENT_COUNT>, ITERS> data;

    // execute for each iteration and collect the data
    for(size_t iter = 0; iter < ITERS; iter++)
        execute_agent_maze<ITERS, AGENT_COUNT, TRIALS, CYCLES>(
                    data,  iter,
                    opt["beta"].as<scalar>(), opt["gamma"].as<scalar>(), opt["tau"].as<scalar>(),
                    opt["method"].as<string>(), opt["fci-method"].as<string>()
                );

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
