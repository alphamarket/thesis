#include "stdafx.hpp"
#include "main.helper.hpp"

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

action action_picker(const Maze_QLearning& mq, const maze::state& state, size_t __unused current_hop, size_t __unused current_iter) {
    // with 20% explore
    if(frand() < .2) return mq.actions_list().at(get_rand(0, mq.actions_list().size()));
    // with 80% exploit
    scalar qprim = -INFINITY;
    action out = 0;
    // search over every action for find the maximum impact in next state
    for(auto act : mq.actions_list()) { if(qprim < mq.Q(state, act)) { qprim = mq.Q(state, act); out = act; } }
    // the argmax action
    return out;
}

maze::state action_handler(const Maze_QLearning& mq, const maze::state& state, action action) {
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
        case maze::NONE:
            return maze::state(state);
        default: throw runtime_error("Undefined action!");
    }
}

QLearningOptions iteration_init_callback(maze& m, size_t __unused iter) {
    set_agent_random(m);
    return {.8, .2};
}

void print_policy(const maze& m, const vector<vector<action>>& policy) {
    for(size_t i = 0; i < m.width; i++) {
        for(size_t j = 0; j < m.height; j++) {
            string s = m.action_tostring(maze::actions(policy[i][j]));
            while(s.length() < 5) s += " ";
            auto color = Color::Modifier(Color::Code::FG_DEFAULT);
            if(m({i,j}).type() == block::WALL)
                color = Color::Modifier(Color::Code::FG_RED);
            else if(m({i,j}).type() == block::GOAL)
                color = Color::Modifier(Color::Code::FG_YELLOW);
            else if(m({i,j}) == Agent())
                color = Color::Modifier(Color::Code::FG_GREEN);
            cout << color << s << Color::Modifier(Color::Code::FG_DEFAULT) << " ";
        }
        cout << endl;
    }
}
