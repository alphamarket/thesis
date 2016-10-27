#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <unordered_map>
#include "matrix.hpp"
#include "colormod.hpp"
#include "world.maze.hpp"
#include "learner.maze.hpp"

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
    foreach_elem(unused a, actions) {
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
