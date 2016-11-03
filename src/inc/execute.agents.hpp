#ifndef EXECUTE_AGENTS_H
#define EXECUTE_AGENTS_H

#include "stdafx.hpp"
#include "agent.hpp"
#include "plugins/plugin_count_hop.hpp"

#include <boost/program_options.hpp>

template<size_t state_dim, size_t action_dim>
void execute_agents(
    vector<agent<state_dim, action_dim>>& agents,
    matrix3D_t<scalar>& data,
    const size_t& CURRENT_ITER,
    const size_t& AGENTS,
    const size_t& TRIALS,
    const size_t& CYCLES,
    const string& method,
    const string& fci_method,
    const boost::program_options::variables_map& opt) {
    matrix1D_t<size_t> hop_sures({AGENTS});
    hop_sures = 0;
    for(size_t trial = 0; trial < TRIALS; trial++) {
        // execute
        for(size_t i = 0; i < AGENTS; i++) agents[i].execute(CYCLES, AGENTS > 1);
        // wait agent to merge
        for(size_t i = 0; i < AGENTS; i++) {
            agents[i].wait_to_execute();
            // check things went as expected
            assert((agents[i].template get_plugin<plugin_count_hop>()->hops.size() - hop_sures[i]) == CYCLES);
            // if az expected? update it
            hop_sures[i] = agents[i].template get_plugin<plugin_count_hop>()->hops.size();
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
        auto h = agents[i].template get_plugin<plugin_count_hop>();
        assert(h->hops.size() == TRIALS * CYCLES);
        // store the data
        std::copy(h->hops.begin(), h->hops.end(), data[CURRENT_ITER][i].begin());
        // release all plugin's resources
        for(auto _plugin : agents[i].get_plugins()) delete _plugin.second;
    }
    // if q table requested?
    if(opt.count("print-qtable-only")) {
        // print one of the agents' Q-Table since in the last TRIAL they are merged
        agents.front().learner->Q.for_each([](const auto& i){ cout << i << " "; });
        cout << endl;
    }
}

#endif // EXECUTE_AGENTS_H
