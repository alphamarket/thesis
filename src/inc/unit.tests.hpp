#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <unordered_map>
#include "colormod.hpp"
#include "world.maze.hpp"
#include "learner.maze.hpp"

void test_world_maze() {
    auto m = maze({3, 4});
    state_t<2> defstate = {1, 1};
    m.set_current_state(defstate);
    assert(m.get_current_state() == defstate);

    vector<pair<maze::moves, state_t<2>>> actions = {
        {maze::UP, {0, 1}},
        {maze::DOWN, {2, 1}},
        {maze::LEFT, {1, 0}},
        {maze::RIGHT, {1, 2}}
    };
    // test the valid movements
    foreach_elem(a, actions) {
        assert(m.make_move({a.first}) == 0);
        assert(m.get_current_state() == a.second);
        m.set_current_state(defstate);
    }
    actions  = {
        {maze::UP, {0, 0}},
        {maze::DOWN, {2, 3}},
        {maze::LEFT, {1, 0}},
        {maze::RIGHT, {1, 3}}
    };
    // the the invalid movement
    foreach_elem(a, actions) {
        m.set_current_state(a.second);
        assert(m.get_current_state() == a.second);
        assert(m.make_move({a.first}) == 0);
        assert(m.get_current_state() == a.second);
    }
}

void test_qleaner() {
    const scalar
        beta = .1,
        gamma = .9,
        reward = 1.;
    learner_maze ql({2, 2, 4});
    assert(ql.update({0, 0}, {0}, {1, 0}, reward, beta, gamma) == 0.1);
    assert(ql.update({0, 0}, {0}, {1, 0}, reward, beta, gamma) == 0.19);
    assert(ql.update({1, 0}, {2}, {1, 1}, reward, beta, gamma) == 0.1);
    assert(ql.advise_action_greedy({0, 0}, 0.)[0] == 0);
    assert(ql.advise_action_boltzmann({0, 0}, 1e-10)[0] == 0);
    assert(ql.advise_action_greedy({1, 0}, 0.)[0] == 2);
    assert(ql.advise_action_boltzmann({1, 0}, 1e-10)[0] == 2);
    assert(ql.advise_action_greedy({1, 0}, 1.)[0] < ql.Q.size().back());
    assert(ql.advise_action_boltzmann({0, 0}, 1e+10)[0] < ql.Q.size().back());
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
