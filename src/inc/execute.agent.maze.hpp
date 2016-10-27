#ifndef EXECUTE_AGENT_MAZE_HPP
#define EXECUTE_AGENT_MAZE_HPP

#include "stdafx.hpp"
#include "inc/agent.hpp"
#include "inc/combine.hpp"
#include "inc/world.maze.hpp"
#include "inc/learner.maze.hpp"
#include "inc/agent.plugins.hpp"

#include "inc/plugins/plugin_sep.hpp"
#include "inc/plugins/plugin_count_hop.hpp"
#include "inc/plugins/plugin_reset_agent.hpp"
#include "inc/plugins/plugin_checkpoints.hpp"

template<size_t ITERS, size_t AGENTS, size_t TRIALS, size_t CYCLES>
void execute_agent_maze(
        array<array<array<scalar, TRIALS * CYCLES>, AGENTS>, ITERS>& data,
        const size_t& CURRENT_ITER,
        const scalar& beta,
        const scalar& gamma,
        const scalar& tau,
        const string& method,
        const string& fci_method) {
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
            new plugin_checkpoints<maze::STATE_DIM, maze::ACTION_DIM>(),
            new plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>(),
            new plugin_SEP<maze::STATE_DIM, maze::ACTION_DIM>(),
            new plugin_reset_agent<maze::STATE_DIM, maze::ACTION_DIM>(),
        });
    }
    // contains previous hops
    array<size_t, AGENTS> hop_sures;
    for(size_t i = 0; i < AGENTS; i++) {
        hop_sures[i] = 0;
        agents.push_back(agent<2, 1>(&worlds[i], &learners[i], beta, gamma, tau));
        foreach_elem(_plugin, plugins[i]) agents.back() += _plugin;
    }
    for(size_t trial = 0; trial < TRIALS; trial++) {
        // execute
        for(size_t i = 0; i < AGENTS; i++) agents[i].execute(CYCLES, AGENTS > 1);
        // wait agent to merge
        for(size_t i = 0; i < AGENTS; i++) {
            agents[i].wait_to_execute();
            // check things went as expected
            assert((agents[i].get_plugin<plugin_count_hop>()->hops.size() - hop_sures[i]) == CYCLES);
            // if az expected? update it
            hop_sures[i] = agents[i].get_plugin<plugin_count_hop>()->hops.size();
        }
        // here we do the combination
        combine(agents, method, fci_method);
    }
    // export the collected data
    for(size_t i = 0; i < AGENTS; i++) {
        auto h = agents[i].get_plugin<plugin_count_hop>();
        assert(h->hops.size() == TRIALS * CYCLES);
        // store the data
        std::copy(h->hops.begin(), h->hops.end(), data[CURRENT_ITER][i].begin());
        // release all plugin's resources
        foreach_elem(_plugin, plugins[i]) delete _plugin;
    }
}

#endif // EXECUTE_AGENT_MAZE_HPP
