#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <unordered_map>

#include "world.maze.hpp"

void test_world_maze() {
    auto m = maze({3, 3});
    state defstate = {1, 1};
    m.set_current_state(defstate);
    assert(m.get_current_state() == defstate);

    vector<pair<maze::moves, state>> actions = {
        {maze::UP, {1, 0}},
        {maze::DOWN, {1, 2}},
        {maze::LEFT, {0, 1}},
        {maze::RIGHT, {2, 1}}
    };

    foreach_elem(a, actions) {
        assert(m.make_move(a.first));
        assert(m.get_current_state() == a.second);
        m.set_current_state(defstate);
    }
}

void execute_tests() {
    unordered_map<string, function<void(void)>> tests = {
        {"world.maze.hpp", test_world_maze}
    };
    foreach_elem(test, tests) test.second();
}

#endif // UNIT_TESTS_HPP
