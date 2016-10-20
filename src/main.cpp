#include <iostream>
#include <inc/worlds/maze.hpp>
#include <inc/worlds/block.hpp>

#define WALL_REWARD -1

int main(int, char**) {
    maze m(6, 6);
    // define the goals positions and theirs rewards(since they can be variable!)
    vector<pair<maze::state, scalar>> goals = {
        {{1, 5}, +10},
        {{3, 5}, +10},
        {{5, 1}, +10}
    };
    // define the walls positions
    vector<maze::state> walls = {
        {1, 0}, {1, 1}, {1, 2},
        {2, 4}, {2, 5},
        {4, 2}, {4, 3}, {4, 4}
    };
    // set the walls' reward
    for(auto g : goals) m(g.first) = block(block::GOAL, g.second);
    // set the walls' reward
    for(auto w : walls) m(w) = block(block::WALL, -1);
    // for every other empty blocks, the reward is
    //  the minimum of the distance of the block to the goals
    m.foreach_block([&goals](maze::state s, block& b) {
        // only process the empty blocks
        if(b.get_type() == b.EMPTY) {
            scalar _min = INFINITY;
            // the block's reward is the minimum of the distance of the block to the goals
            for(auto g : goals) _min = min((g.first - s).norm(), _min);
            // set the block's reward
            b.set_reward(1 / _min);
        }
        return true;
    });

    cout << m;

	return 0;
}
