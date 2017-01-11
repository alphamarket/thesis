#include "inc/stdafx.hpp"
#include "inc/unit.tests.hpp"
#include "inc/main.extend.hpp"
#include "inc/execute.agent.maze.hpp"
#include "inc/execute.agent.prey.hpp"

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
    // obtain the args
    auto opt = process_args(argc, argv);

    const size_t ITERS = opt["iters"].as<size_t>();
    const size_t CYCLES = opt["cycles"].as<size_t>();
    const size_t TRIALS = opt["trials"].as<size_t>();
    const size_t AGENT_COUNT = opt["agents"].as<size_t>();

    fprintf(stderr, "Method: %s\nEnvironment: %s\nRL Action Advisor: %s\n", opt["method"].as<string>().c_str(), opt["env"].as<string>().c_str(), opt["rl-action-advisor"].as<string>().c_str());
    fprintf(stderr, "Running %zu agents with %zu iterations %zu times ....\n", AGENT_COUNT, TRIALS * CYCLES, ITERS);

    // ITERS x AGENTS x [TRIALS * CYCLES]
    matrix3D_t<scalar> data({ ITERS, AGENT_COUNT, TRIALS * CYCLES });

    // execute for each iteration and collect the data
    for(size_t iter = 0; iter < ITERS; iter++)
        if(opt["env"].as<string>() == "maze")
            execute_agent_maze(
                        data,  iter,
                        AGENT_COUNT, TRIALS, CYCLES,
                        opt["beta"].as<scalar>(), opt["gamma"].as<scalar>(), opt["tau"].as<scalar>(), opt["epsilon"].as<scalar>(),
                        opt["method"].as<string>(), opt["fci-combinator"].as<string>(),
                        opt["refmat-grind"].as<size_t>(),
                        opt
                    );
        else
            execute_agent_prey(
                        data,  iter,
                        AGENT_COUNT, TRIALS, CYCLES,
                        opt["beta"].as<scalar>(), opt["gamma"].as<scalar>(), opt["tau"].as<scalar>(), opt["epsilon"].as<scalar>(),
                        opt["method"].as<string>(), opt["fci-combinator"].as<string>(),
                        opt["refmat-grind"].as<size_t>(),
                        opt
                    );
    // if didn't ask for printing Q-Tables? then print avg. moves
    if(opt["output"].as<string>() == "avg-moves") {
        // try to average data over AGENTS and ITERs
        matrix1D_t<scalar> avg_data({TRIALS * CYCLES});
        for(size_t k = 0; k < TRIALS * CYCLES; k++) {
            for(size_t i = 0; i < ITERS; i++) {
                for(size_t j = 0; j < AGENT_COUNT; j++) {
                    avg_data[k] += data[i][j][k] / (AGENT_COUNT * ITERS);
                }
            }
        }

        // print accumulate the sums of avg data
        for(size_t i = 0; i < avg_data.num_elements(); i++)
            cout << accumulate(avg_data.begin(), avg_data.begin() + i + 1, 0.0) / (i + 1) << " ";
    }

    // exit the program
    exiting = true;
    while(!thread_pool.empty()) { thread_pool.back().get(); thread_pool.pop_back(); }
    exit(EXIT_SUCCESS);
}
