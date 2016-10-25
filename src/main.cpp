#include "inc/stdafx.hpp"
#include "inc/agent.hpp"
#include "inc/world.maze.hpp"
#include "inc/unit.tests.hpp"
#include "inc/learner.maze.hpp"

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {
    execute_tests();
    vector<agent<2, 1>> agents;
    maze world({2, 2});
    learner_maze leaner({2, 2, 4});
    for(size_t i = 0; i < 3; i++) agents.push_back(agent<2, 1>(&world, &leaner));

    auto start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < agents.size(); i++) agents[i].execute(500, true);
    for(size_t i = 0; i < agents.size(); i++) agents[i].wait_to_execute();
    cerr << "Threading time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << endl;

    start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < agents.size(); i++) agents[i].execute(500, false);
    for(size_t i = 0; i < agents.size(); i++) agents[i].wait_to_execute();
    cerr << "Simple time: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << endl;

    exit(EXIT_SUCCESS);
}
