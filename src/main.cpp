#include "inc/stdafx.hpp"
#include "inc/agent.hpp"
#include "inc/world.maze.hpp"
#include "inc/unit.tests.hpp"
#include "inc/learner.maze.hpp"

#define AGENT_COUNT 2

class print_maze_world : public IPlugin {

    inline agent<2, 1>* get_agent(void* const in) const { return static_cast<agent<2, 1>*>(in); }

    virtual void notify_deinit_event(void* const in) {
        auto a = this->get_agent(in);
        auto p = a->learner->get_policy();

        cerr << "------------------" << endl;
        for(size_t i = 0; i < a->world->size()[0]; i++) {
            for(size_t j = 0; j < a->world->size()[1]; j++) {
                auto color = Color::Modifier(Color::FG_DEFAULT);
                auto ss = a->world->get_current_state();
                if(ss[0] == i && ss[1] == j) color = Color::Modifier(Color::FG_GREEN);
                if(!a->world->get_maze()[i][j]._is_moveable) color = Color::Modifier(Color::FG_RED);
                if(a->world->get_maze()[i][j]._value > 0) color = Color::Modifier(Color::FG_YELLOW);
                assert(a->world->get_current_block()._is_moveable);
                cerr << color << a->world->move_tostring({p({i, j})})[0] << " " << Color::Modifier(Color::FG_DEFAULT);
            }
            cerr << endl;
        }
        cerr << endl << "***************" << endl;
    }

    virtual void notify_enter_event(void* const) { }

    virtual void notify_pre_event(void* const) { }

    virtual void notify_on_event(void* const) { }

    virtual void notify_init_event(void* const in) {
        auto a = this->get_agent(in);
        a->world->set_current_state({get_rand(0, a->world->size()[0]), get_rand(0, a->world->size()[1])});
    }

    virtual void notify_post_event(void* const) { }

    virtual void notify_exit_event(void* const) { }
};

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {
    // exiting flag
    bool exiting = false;
    vector<future<void>> thread_pool;
    // random seed updator
    thread_pool.push_back(std::async(std::launch::async, [&exiting]() { while(!exiting) { updateseed(); std::this_thread::sleep_for(std::chrono::milliseconds(100));}} ));

    execute_tests();
    vector<maze> worlds;
    vector<agent<2, 1>> agents;
    vector<learner_maze> learners;
    for(size_t i = 0; i < AGENT_COUNT; i++) {
        worlds.push_back(maze({6, 6}));
        worlds.back().define_values({
            // define a goal
            {{1, 5}, block(10, 1, 1)},
            {{3, 5}, block(10, 1, 1)},
            {{5, 0}, block(10, 1, 1)},
            // define walls
            {{1, 0}, block(-1, 0, 0)},
            {{1, 1}, block(-1, 0, 0)},
            {{1, 2}, block(-1, 0, 0)},
            {{2, 4}, block(-1, 0, 0)},
            {{2, 5}, block(-1, 0, 0)},
            {{4, 2}, block(-1, 0, 0)},
            {{4, 3}, block(-1, 0, 0)},
            {{4, 4}, block(-1, 0, 0)},
        });
        learners.push_back(learner_maze({worlds.back().size()[0], worlds.back().size()[1], worlds.back().action_no()}));
    }

    for(size_t i = 0; i < AGENT_COUNT; i++) {
        agents.push_back(agent<2, 1>(&worlds[i], &learners[i]));
        agents.back() += {"maze printer", new print_maze_world()};
    }
    for(size_t i = 0; i < agents.size(); i++)
        agents[i].execute(100, agents.size() > 1);
    for(size_t i = 0; i < agents.size(); i++)
        agents[i].wait_to_execute();

    exiting = true;
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    exit(EXIT_SUCCESS);
}
