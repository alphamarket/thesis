#include "inc/execute.agent.prey.hpp"

void execute_agent_prey(
        matrix3D_t<scalar>& data,
        const size_t& CURRENT_ITER,
        const size_t& AGENTS,
        const size_t& TRIALS,
        const size_t& CYCLES,
        const scalar& beta,
        const scalar& gamma,
        const scalar& tau,
        const string& method,
        const string& fci_method,
        const size_t& grind,
        const boost::program_options::variables_map& opt) {
    flag_workflow();
    vector<hunter_prey> worlds;
    vector<learner_hunter> learners;
    vector<agent<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>> agents;
    vector<vector<plugin<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>*>> plugins;
    // define new values for each agent
    for(size_t i = 0; i < AGENTS; i++) {
        worlds.push_back(hunter_prey());
        // define a goal
        worlds.back().define_values({
            {{1}, block(10, 1, 1)},
            {{5}, block(10, 1, 1)},
            {{9}, block(10, 1, 1)},
            {{13}, block(10, 1, 1)},
        });
        for(size_t i = 0; i < worlds.back().size()[0]; i++) {
            if(i != 1 && i != 5 && i != 9 && i != 13)
                worlds.back().define_values({{{i}, block(0.1, 0, 1)}});
        }
        // define the new learner
        learners.push_back(learner_hunter({worlds.back().size()[0], worlds.back().action_no()}));
        // add default plugins
        plugins.push_back({
            new plugin_checkpoints<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>(),
            new plugin_count_hop<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>()
        });
        // if plugin SEP requested?
        if(method == "sep" || method == "sep-refmat")
            plugins.back().push_back(new plugin_SEP<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>());
        if(method == "refmat" || method == "sep-refmat")
            plugins.back().push_back(new plugin_reference_counter<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>(grind));
        // needs to be added after plugin_SEP
        plugins.back().push_back(new plugin_reset_agent<hunter_prey::STATE_DIM, hunter_prey::ACTION_DIM>());
    }
    // contains previous hops
    matrix1D_t<size_t> hop_sures({AGENTS});
    for(size_t i = 0; i < AGENTS; i++) {
        hop_sures[i] = 0;
        agents.push_back(agent<hunter_prey::STATE_DIM, hunter_prey::STATE_DIM>(&worlds[i], &learners[i], beta, gamma, tau));
        for(auto _plugin : plugins[i]) agents.back() += _plugin;
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
        if(AGENTS > 1 || method == "sep")
            // here we do the combination
            combine(agents, method, fci_method);
        if(opt.count("print-qtable-max"))
            cout << agents.front().learner->Q.max() << " ";
    }
    if(opt.count("print-qtable-max"))
        cout << endl;
    // export the collected data
    for(size_t i = 0; i < AGENTS; i++) {
        auto h = agents[i].get_plugin<plugin_count_hop>();
        assert(h->hops.size() == TRIALS * CYCLES);
        // store the data
        std::copy(h->hops.begin(), h->hops.end(), data[CURRENT_ITER][i].begin());
        // release all plugin's resources
        for(auto _plugin : plugins[i]) delete _plugin;
    }
    // if q table requested?
    if(opt.count("print-qtable-only")) {
        // print one of the agents' Q-Table since in the last TRIAL they are merged
        agents.front().learner->Q.for_each([](const auto& i){ cout << i << " "; });
        cout << endl;
    }
}
