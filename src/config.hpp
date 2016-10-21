#ifndef CONFIG_HPP
#define CONFIG_HPP

#define MAZE_WALL_REWARD -1

#define MULTI_AGENT_COUNT       3
#define AGENT_LEARNING_CYCLES   5
#define TRIAL_MAX               200

#define QLEARNING_ALPHA         .10
#define QLEARNING_GAMMA         .90

#define ACTION_PICKER_BOLTSMAN_TAU      .2
#define ACTION_PICKER_GREEDY_EXPLORE    .4

#endif // CONFIG_HPP
