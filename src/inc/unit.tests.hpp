#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <unordered_map>
#include "colormod.hpp"
#include "qlearner.hpp"
#include "world.maze.hpp"

void test_world_maze() {
    auto m = maze({3, 4});
    state defstate = {1, 1};
    m.set_current_state(defstate);
    assert(m.get_current_state() == defstate);

    vector<pair<maze::moves, state>> actions = {
        {maze::UP, {1, 0}},
        {maze::DOWN, {1, 2}},
        {maze::LEFT, {0, 1}},
        {maze::RIGHT, {2, 1}}
    };
    // test the valid movements
    foreach_elem(a, actions) {
        assert(m.make_move(a.first));
        assert(m.get_current_state() == a.second);
        m.set_current_state(defstate);
    }
    actions  = {
        {maze::UP, {0, 0}},
        {maze::DOWN, {2, 3}},
        {maze::LEFT, {0, 2}},
        {maze::RIGHT, {2, 1}}
    };
    // the the invalid movement
    foreach_elem(a, actions) {
        m.set_current_state(a.second);
        assert(m.get_current_state() == a.second);
        assert(!m.make_move(a.first));
        assert(m.get_current_state() == a.second);
    }
}

void test_qleaner() {
    const scalar
        beta = .1,
        gamma = .9,
        reward = 1.;
    maze_learner ql({2, 2, 4});
    assert(ql.update({0, 0, 0}, {1, 0}, reward, beta, gamma) == 0.1);
    assert(ql.update({0, 0, 0}, {1, 0}, reward, beta, gamma) == 0.19);
    assert(ql.update({1, 0, 2}, {1, 1}, reward, beta, gamma) == 0.1);
    assert(ql.advise_action_greedy({0, 0}, 0.) == 0);
    assert(ql.advise_action_boltzmann({0, 0}, 1e-10) == 0);
    assert(ql.advise_action_greedy({1, 0}, 0.) == 2);
    assert(ql.advise_action_boltzmann({1, 0}, 1e-10) == 2);
    assert(ql.advise_action_greedy({1, 0}, 1.) < ql.Q.size().back());
    assert(ql.advise_action_boltzmann({0, 0}, 1e+10) < ql.Q.size().back());
}

void execute_tests() {
    unordered_map<string, function<void(void)>> tests = {
        {"world.maze.hpp", test_world_maze},
        {"qlearner.hpp", test_qleaner}
    };
    cerr << Color::Modifier(Color::FG_YELLOW) << "Engaging the tests..." << Color::Modifier(Color::FG_DEFAULT) << endl;
    foreach_elem(test, tests) {
        cerr << Color::Modifier(Color::FG_YELLOW) << "  [.] " << test.first << Color::Modifier(Color::FG_DEFAULT);
        test.second();
        cerr << Color::Modifier(Color::FG_GREEN) << "\r  [OK] " << Color::Modifier(Color::FG_DEFAULT) << test.first << endl;
    }
}

#endif // UNIT_TESTS_HPP
