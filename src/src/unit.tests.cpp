#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include "inc/unit.tests.hpp"

#include "inc/matrix.hpp"
#include "inc/colormod.hpp"
#include "inc/world.maze.hpp"
#include "inc/world.prey.hpp"
#include "inc/learner.maze.hpp"

#include <unordered_map>

void test_matrix() {
    // testing the general functionalities
    {
        matrix<size_t, 2> m({2, 3});
        assert(m.size().size() == 2 && m.size()[0] == 2 && m.size()[1] == 3);
        assert(m == size_t());
        m = 2;
        assert(m == 2 && m != 3 && m < 3 && m <= 4 && m <=2 && m > 1 && m >= 0 && m >= 2);
        m += 1;
        assert(m == 3);
        m -= 3;
        assert(m == 0 && m == size_t());
        m = 10, m /= 2;
        assert(m == 5);
        m *= 3;
        assert(m == 15);
        auto b = m;
        assert(m == b && m <= b && m >= b);
        b = 13, m = 14;
        assert(m == 14 && b == 13 && m >= b && m > b && b < m && b <= m);
        assert(m.size().size() == 2 && m.size()[0] == 2 && m.size()[1] == 3);
        assert(b.size().size() == 2 && b.size()[0] == 2 && b.size()[1] == 3);
        assert(
            (m / b) == (14 / 13) &&
            (b / m) == (13 / 14) &&
            (m.as<scalar>() / b) == (14 / 13.) &&
            (m * b )== (14 * 13) &&
            (b + m) == (13 + 14) &&
            (m - b) == (14 - 13) &&
            (b.as<scalar>() - m) == (13 - 14) &&
            (m += b) == (13 + 14) &&
            (m -= b) == (14) &&
            (m *= b) == (14 * 13) &&
            (m /= b) == (14));
        assert(b.min() == 13 && b.max() == 13);
        m[0][1] = 1;
        assert(m.min() == 1 && m.max() == 14);
        matrix<scalar, 2> c = b.as<scalar>();
        assert(c == 13);
        c[0][2] = -1;
        assert(c.min() == -1 && c.max() == 13 && c.abs() >= 0 && c[0][2] == -1);
        assert(c.norm2() == ((13*13)*5 + 1));
    }
    // test the slicing capablity
    {
        matrix<scalar, 3> m({3, 4, 4});
        m = 13;
        assert(m == 13);
        auto s = m.slice(slice<2>({2, 3}));
        assert(s.size().size() == 1 && s.size()[0] == 4 && s == 13);
        s = 14;
        assert(s == 14 && m.slice(slice<2>({2, 3})) == 13 && m == 13);
        m.slice_ref(slice<2>({2, 3})).for_each([](scalar* const _s) { *_s = 14; return true; });
        assert(m != 13 && m.slice(slice<1>({2})) != 14 && m.slice(slice<2>({2, 3})) == 14 && m.slice(slice<1>({0})) == 13 && m.slice(slice<1>({1})) == 13);
        for(size_t i = 0; i < 3; i++)
            for(size_t j = 0; j < 4; j++)
                for(size_t k = 0; k < 4; k++)
                    if(i == 2 && j == 3) assert(m({i, j, k}) == 14);
                    else assert(m({i, j, k}) == 13);
    }
    // test listing indices
    {
        matrix<size_t, 2> m({2, 3});
        assert(m.list_indices<1>().size() == 2);
        for(size_t i = 0; i < m.size()[0]; i++)
            assert(m.list_indices<1>()[i][0] == i);
        auto l = m.list_indices();
        assert(l.size() == 6);
        for(size_t i = 0, k = 0; i < m.size()[0]; i++)
            for(size_t j = 0; j < m.size()[1]; j++, k++)
                assert(l[k].size() == m.size().size() && l[k][0] == i && l[k][1] == j);
    }
}

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
    for(auto unused a : actions) {
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
    for(auto a : actions) {
        m.set_current_state(a.second);
        assert(m.get_current_state() == a.second);
        assert(m.make_move({a.first}) == 0);
        assert(m.get_current_state() == a.second);
    }
}

void test_world_prey() {
    hunter_prey hp;
    array<scalar, 2> defstate = {5, 6};
    hp.set_hunter_location(defstate);
    hp.set_prey_location({defstate[0] + .25, defstate[1] + .25});
    // since they are almost at the same position, the state should be 1
    assert(hp.get_current_state()[0] == 1);
    assert(hp.get_prey_location()[0] == defstate[0] + .25 && hp.get_prey_location()[1] == defstate[1] + .25);
    assert(hp.get_hunter_location()[0] == defstate[0] && hp.get_hunter_location()[1] == defstate[1]);
    vector<pair<hunter_prey::moves, array<scalar, 2>>> actions = {
        {hunter_prey::m0_05, {5.5, 6}},
        {hunter_prey::m0_10, {6, 6}},
        {hunter_prey::m45_05, {5.5, 6.5}},
        {hunter_prey::m45_10, {6, 7}},
        {hunter_prey::m90_05, {5, 6.5}},
        {hunter_prey::m90_10, {5, 7}},
        {hunter_prey::m135_05, {4.5, 6.5}},
        {hunter_prey::m135_10, {4, 7}},
        {hunter_prey::m180_05, {4.5, 6}},
        {hunter_prey::m180_10, {4, 6}},
        {hunter_prey::m225_05, {4.5, 5.5}},
        {hunter_prey::m225_10, {4, 5}},
        {hunter_prey::m270_05, {5, 5.5}},
        {hunter_prey::m270_10, {5, 5}},
        {hunter_prey::m315_05, {5.5, 5.5}},
        {hunter_prey::m315_10, {6, 5}}
    };
    // test the valid movements
    for(auto unused a : actions) {
        assert(hp.make_move({a.first}) == 0);
        assert(hp.get_hunter_location()[0] == a.second[0] && hp.get_hunter_location()[1] == a.second[1]);
        hp.set_hunter_location(defstate);
    }
    vector<pair<array<scalar, 2>, pair<hunter_prey::moves, array<scalar, 2>>>> limit_actions = {
        // down
        {{0, 0}, {hunter_prey::m270_05, {0, 0}}},
        // left
        {{0, 0}, {hunter_prey::m180_05, {0, 0}}},
        // right
        {{10, 10}, {hunter_prey::m0_05, {10, 10}}},
        // up
        {{10, 10}, {hunter_prey::m90_05, {10, 10}}},
    };
    // test the invalid movements
    for(auto unused a : limit_actions) {
        hp.set_hunter_location(a.first);
        assert(hp.make_move({a.second.first}) == 0);
        assert(hp.get_hunter_location()[0] == a.second.second[0] && hp.get_hunter_location()[1] == a.second.second[1]);
    }
    // foreach hunter move the prey moves randomly too,
    // since it's random there no need to test the movement, which can fail randomly
    vector<pair<pair<array<scalar, 2>, array<scalar, 2>>, size_t>> states = {
        {{{5, 5}, {5.25, 5.25}}, 1},
        {{{5, 5}, {5.75, 5.75}}, 2},
        {{{5, 5}, {6.25, 6.25}}, 3},
        {{{5, 5}, {6.75, 6.75}}, 4},

        {{{5, 5}, {5.25, 4.75}}, 5},
        {{{5, 5}, {5.75, 4.25}}, 6},
        {{{5, 5}, {6.25, 3.75}}, 7},
        {{{5, 5}, {6.75, 3.25}}, 8},

        {{{5, 5}, {4.75, 4.75}}, 9},
        {{{5, 5}, {4.25, 4.25}}, 10},
        {{{5, 5}, {3.75, 3.75}}, 11},
        {{{5, 5}, {3.25, 3.25}}, 12},

        {{{5, 5}, {4.75, 5.25}}, 13},
        {{{5, 5}, {4.25, 5.75}}, 14},
        {{{5, 5}, {3.75, 6.25}}, 15},
        {{{5, 5}, {3.25, 6.75}}, 16},

        {{{5, 5}, {6.00, 7.00}}, 4},

        {{{5, 5}, {5.00, 7.25}}, 0},
        {{{5, 5}, {1, 1}}, 0},
        {{{5, 5}, {9, 9}}, 0},
        {{{5, 5}, {3, 9}}, 0},
    };
    // test states depending on locations
    for(auto unused a : states) {
        hp.set_prey_location(a.first.second);
        hp.set_hunter_location(a.first.first);
        assert(hp.get_current_state()[0] == a.second);
    }
}

void test_qleaner() {
    const unused scalar
        beta = .1,
        gamma = .9,
        reward = 1.;
    unused learner_maze ql({2, 2, 4});
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
    vector<pair<string, function<void(void)>>> tests = {
        {"matrix.hpp", test_matrix},
        {"world.maze.hpp", test_world_maze},
        {"world.prey.hpp", test_world_prey},
        {"qlearner.hpp", test_qleaner}
    };
//    cerr << Color::Modifier(Color::FG_YELLOW) << "Engaging the tests..." << Color::Modifier(Color::FG_DEFAULT) << endl;
    for(auto test : tests) {
//        cerr << Color::Modifier(Color::FG_YELLOW) << "  [.] " << test.first << Color::Modifier(Color::FG_DEFAULT);
        test.second();
//        cerr << Color::Modifier(Color::FG_GREEN) << "\r  [OK] " << Color::Modifier(Color::FG_DEFAULT) << test.first << endl;
    }
}

#endif // UNIT_TESTS_HPP
