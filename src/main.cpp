#include <iostream>
#include <inc/worlds/maze.hpp>
#include <inc/rl/maze_qlearning.hpp>

#define WALL_REWARD -1

/**
 * @brief create_maze Creates a maze world
 * @param size The size of the world
 * @param goals The vector of pairs position and reward of goals
 * @param walls The position of walls
 * @return The created and initalized maze
 */
maze create_maze(
        const maze::state& size,
        vector<pair<maze::state, scalar>> goals,
        vector<maze::state> walls);

/**
 * @brief set_agent_random Sets the agent at valid random position
 * @param m The maze to act upon
 */
inline void set_agent_random(maze& m);

/**
 * @brief action_picker Picks an action
 * @param mq The Maze QLearning instance
 * @param current_hop The current hop#
 * @param current_iter The current iter#
 * @return The picked action
 */
Maze_QLearning::action action_picker(const Maze_QLearning& mq, const maze::state &state, size_t current_hop, size_t current_iter);
/**
 * @brief action_handler The action handler
 * @param mq The Maze QLearning instance
 * @param state The current state
 * @param action The action to be taken
 * @return The transited state
 */
maze::state action_handler(const Maze_QLearning& mq, const maze::state& state, Maze_QLearning::action action);

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {

    maze m = create_maze(
                // the size of maze
                {6, 7},
                // define the goals positions and theirs rewards(since they can be variable!)
                {
                    {{1, 5}, +10},
                    {{3, 5}, +10},
                    {{5, 1}, +10}
                },
                // define the walls positions
                {
                    {1, 0}, {1, 1}, {1, 2},
                    {2, 4}, {2, 5},
                    {4, 2}, {4, 3}, {4, 4}
                });
    set_agent_random(m);
    Maze_QLearning mq(m, {maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT});
//    mq.execute(action_picker, action_handler);
    cout << m << endl;
    for(auto a : {maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT}){
        cout << a << "? " << m.agent_location(action_handler(mq, m.agent_location(), a)) << endl;
        cout << m << endl;
    }

	return 0;
}

maze create_maze(
        const maze::state& size,
        vector<pair<maze::state, scalar>> goals,
        vector<maze::state> walls)
{
    maze m(size[0], size[1]);
    // set the walls' reward
    for(auto g : goals) m(g.first) = block(block::GOAL, g.second);
    // set the walls' reward
    for(auto w : walls) m(w) = block(block::WALL, WALL_REWARD);
    // for every other empty blocks, the reward is
    //  the minimum of the distance of the block to the goals
    m.foreach_block([&goals](maze::state s, block& b) {
        // only process the empty blocks
        if(b.type() == b.EMPTY) {
            scalar _min = INFINITY;
            // the block's reward is the minimum of the distance of the block to the goals
            for(auto g : goals) _min = min(scalar((g.first - s).norm()), _min);
            // set the block's reward
            b.reward(1 / _min);
        }
        return true;
    });
    return m;
}

void set_agent_random(maze& m) {
    updateseed();
    maze::state s;
    do {
        s = {get_rand(0, m.width), get_rand(0, m.height)};
        m.agent_location(s);
    } while(m(s).type() != block::EMPTY);
}

Maze_QLearning::action action_picker(const Maze_QLearning& mq, const maze::state& state, size_t __unused current_hop, size_t __unused current_iter) {
    // with 20% explore
    if(frand() < .2) return mq.actions_list().at(get_rand(0, mq.actions_list().size()));
    // with 80% exploit
    scalar qprim = -INFINITY;
    Maze_QLearning::action out = 0;
    // search over every action for find the maximum impact in next state
    for(auto act : mq.actions_list()) { if(qprim < mq.Q(state, act)) { qprim = mq.Q(state, act); out = act; } }
    // the argmax action
    return out;
}

maze::state action_handler(const Maze_QLearning& mq, const maze::state& state, Maze_QLearning::action action) {
    switch(action) {
        case maze::TOP:
            if(state[0] > 0) return {state[0] - 1, state[1]};
            return maze::state(state);
        case maze::RIGHT:
            if(size_t(state[1]) < mq.get_maze().height - 1) return {state[0], state[1] + 1};
            return maze::state(state);
        case maze::DOWN:
            if(size_t(state[0]) < mq.get_maze().width - 1) return {state[0] + 1, state[1]};
            return maze::state(state);
        case maze::LEFT:
            if(state[1] > 0) return {state[0], state[1] - 1};
            return maze::state(state);
        default: throw runtime_error("Undefined action!");
    }
}
