#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "stdafx.hpp"

extern scalar

CONF_RATE_TAU,
CONF_RATE_BETA,
CONF_RATE_GAMMA,
CONF_MAZE_WALL_REWARD,
CONF_RATE_GREEDY_EXPLORE

;

extern size_t

CONF_TRIAL_MAX,
CONF_ITERATIONS,
CONF_MULTI_AGENT_COUNT,
CONF_AGENT_LEARNING_CYCLES

;

struct fci_choquet_package;

typedef function<scalar(const vector<fci_choquet_package>&, const size_t&)> fci_combiner_func_t;

extern fci_combiner_func_t CONF_FCI_COMBINER;

#endif // CONFIG_HPP
