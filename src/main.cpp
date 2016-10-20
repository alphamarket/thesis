#include <iostream>
#include <inc/worlds/maze.hpp>
#include <inc/worlds/block.hpp>

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
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {

    maze m = create_maze(
                // the size of maze
                {6, 6},
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

    for (auto __unused i : {1,2,3}){
        set_agent_random(m);
        cout << m <<endl;
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
            for(auto g : goals) _min = min((g.first - s).norm(), _min);
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
        m.set_agent(s);
    } while(m(s).type() != block::EMPTY);
}
