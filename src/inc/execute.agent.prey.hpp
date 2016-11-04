#ifndef EXECUTE_AGENT_PREY_HPP
#define EXECUTE_AGENT_PREY_HPP


#include "inc/stdafx.hpp"
#include "inc/agent.hpp"
#include "inc/combine.hpp"
#include "inc/world.prey.hpp"
#include "inc/agent.plugins.hpp"
#include "inc/learner.hunter.hpp"
#include "inc/plugins/plugin_sep.hpp"
#include "inc/plugins/plugin_count_hop.hpp"
#include "inc/plugins/plugin_reset_agent.hpp"
#include "inc/plugins/plugin_checkpoints.hpp"
#include "inc/plugins/plugin_reference_counter.hpp"

#include <boost/program_options.hpp>

void execute_agent_prey(matrix3D_t<scalar>& data,
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
        const boost::program_options::variables_map&);

#endif // EXECUTE_AGENT_PREY_HPP
