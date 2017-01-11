#include "inc/execute.agent.maze.hpp"
#include "inc/execute.agents.hpp"

void execute_agent_maze(matrix3D_t<scalar>& data,
        const size_t& CURRENT_ITER,
        const size_t& AGENTS,
        const size_t& TRIALS,
        const size_t& CYCLES,
        const scalar& beta,
        const scalar& gamma,
        const scalar& tau,
        const scalar& epsilon,
        const string& method,
        const string& fci_method,
        const size_t& grind,
        const boost::program_options::variables_map& opt) {
    flag_workflow();
    vector<maze> worlds;
    vector<learner_maze> learners;
    vector<agent<maze::STATE_DIM, maze::ACTION_DIM>> agents;
    vector<vector<plugin<maze::STATE_DIM, maze::ACTION_DIM>*>> plugins;
    auto norm = [](const state_t<2>& i, const state_t<2>& j) { return sqrt(pow(i[0] - j[0], 2) + pow(i[1] - j[1], 2)); };
    // define new values for each agent
    for(size_t i = 0; i < AGENTS; i++) {
        worlds.push_back(maze({6, 6}));
        vector<pair<state_t<2>, block>> _goals = {
            {{1, 5}, block(10, 1, 1)},
            {{3, 5}, block(10, 1, 1)},
            {{5, 0}, block(10, 1, 1)},
        };
        // define a goal
        worlds.back().define_values(_goals);
        // define walls
        worlds.back().define_values({
            {{1, 0}, block(-1, 0, 0)},
            {{1, 1}, block(-1, 0, 0)},
            {{1, 2}, block(-1, 0, 0)},
            {{2, 4}, block(-1, 0, 0)},
            {{2, 5}, block(-1, 0, 0)},
            {{4, 2}, block(-1, 0, 0)},
            {{4, 3}, block(-1, 0, 0)},
            {{4, 4}, block(-1, 0, 0)},
        });
        for(auto ii : worlds.back().get_maze().list_indices()) {
            block* b = &worlds.back().get_maze()(ii);
            if(b->_value == 0) {
                scalar _min = numeric_limits<scalar>::infinity();
                for(auto g : _goals) _min = min(norm(g.first, ii), _min);
                b->_value = 1 / _min;
            }
        }
        // define the new learner
        learners.push_back(learner_maze({worlds.back().size()[0], worlds.back().size()[1], worlds.back().action_no()}));
        // add default plugins
        plugins.push_back({
            new plugin_checkpoints<maze::STATE_DIM, maze::ACTION_DIM>(),
            new plugin_count_hop<maze::STATE_DIM, maze::ACTION_DIM>()
        });
        // if plugin SEP requested?
        if(method == "sep" || method == "sep-refmat")
            plugins.back().push_back(new plugin_SEP<maze::STATE_DIM, maze::ACTION_DIM>());
        if(method == "refmat" || method == "sep-refmat")
            plugins.back().push_back(new plugin_reference_counter<maze::STATE_DIM, maze::ACTION_DIM>(grind));
        if(method == "mce")
            plugins.back().push_back(new plugin_MCE<maze::STATE_DIM, maze::ACTION_DIM>());
        // needs to be added after plugin_SEP
        plugins.back().push_back(new plugin_reset_agent<maze::STATE_DIM, maze::ACTION_DIM>());
    }
    // contains previous hops
    for(size_t i = 0; i < AGENTS; i++) {
        agents.push_back(agent<maze::STATE_DIM, maze::ACTION_DIM>(&worlds[i], &learners[i], opt["rl-action-advisor"].as<string>(), beta, gamma, tau, epsilon));
        for(auto _plugin : plugins[i]) agents.back() += _plugin;
    }
    // execute the agents
    execute_agents(agents, data, CURRENT_ITER, AGENTS, TRIALS, CYCLES, method, fci_method, opt);
}
