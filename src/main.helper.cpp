#include "stdafx.hpp"
#include "main.helper.hpp"
#include <unordered_map>

maze create_maze(
        const maze::state& size,
        const size_t ref_size,
        vector<pair<maze::state, scalar>> goals,
        vector<maze::state> walls)
{
    maze m(size[0], size[1], ref_size);
    // set the walls' reward
    for(auto g : goals) m(g.first) = block(block::GOAL, g.second);
    // set the walls' reward
    for(auto w : walls) m(w) = block(block::WALL, ::CONF_MAZE_WALL_REWARD);
    // for every other empty blocks, the reward is
    //  the minimum of the distance of the block to the goals
    m.foreach_block([&goals](maze::state s, block& b) {
        // only process the empty blocks
        if(b.type() == b.EMPTY) {
            scalar _min = INFINITY;
            // the block's reward is the minimum of the distance of the block to the goals
            for(auto g : goals) _min = min(scalar((g.first - s).norm()), _min);
            // set the block's reward
            b.value(1 / _min);
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

action greedy_action_picker(const Maze_QLearning& mq, const maze::state& state, size_t __unused current_hop, size_t __unused current_iter) {
    // with 20% explore
    if(frand() < ::CONF_RATE_GREEDY_EXPLORE) return mq.actions_list().at(get_rand(0, mq.actions_list().size()));
    // with 80% exploit
    scalar qprim = -INFINITY;
    action out = 0;
    // search over every action for find the maximum impact in next state
    foreach_elem(act, mq.actions_list()) { if(qprim < mq.Q(state, act)) { qprim = mq.Q(state, act); out = act; } }
    // the argmax action
    return out;
}

action boltzmann_action_picker(const Maze_QLearning& mq, const maze::state &state, size_t __unused current_hop, size_t __unused current_iter) {
    scalar sum = 0;
    size_t index = 0;
    scalar p = frand();
    vector<pair<size_t, scalar>> eq;
    // calc the exp(Q/tau)
    foreach_elem(act, mq.actions_list()) { eq.push_back({index++, exp(mq.Q(state, act) / ::CONF_RATE_TAU)}); sum += eq.back().second; }
    // normalize them
    foreach_elem(&e, eq) e.second /= sum;
    // sort in ascending order
    sort(eq.begin(), eq.end(), [](auto i, auto j) { return i.second < j.second; });
    sum = 0;
    foreach_elem(e, eq) {
        if(p < sum + e.second) return mq.actions_list()[e.first];
        sum += e.second;
    }
    throw runtime_error("The PC should not reach this!");
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
    return {::CONF_RATE_BETA, ::CONF_RATE_GAMMA};
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

QLearningResult execute_agent(
        maze m,
        const vector<action>& action_list,
        const qtable_t& qtable,
        Maze_QLearning::action_func_t action_picker,
        Maze_QLearning::qupdate_func_t greedy_qupdator,
        size_t iteration_max,
        size_t __unused thread_id) {
    set_agent_random(m);
    Maze_QLearning mq(qtable, m, action_list);
    return mq.execute(action_picker, action_handler, greedy_qupdator, iteration_init_callback, iteration_max);
}

vector<scalar> get_avg_hop_single(const vector<QLearningResult>& results) {
    vector<scalar> out(results[0]._hops.size(), 0.);
    foreach_agent(tid) {
        assert(out.size() == results[tid]._hops.size());
        for(size_t i = 0; i < results[tid]._hops.size(); i++)
            out[i] += results[tid]._hops[i] / ::CONF_MULTI_AGENT_COUNT;
    }
    return out;
}

vector<scalar> get_avg_hop_clustered(const vector<QLearningResult> &results) {
    vector<scalar> out = { 0 };
    foreach_agent(tid) {
        auto hop = results[tid]._hops;
        out.back() += (accumulate(hop.begin(), hop.end(), 0.0) / (hop.size() * ::CONF_MULTI_AGENT_COUNT));
    }
    return out;
}

scalar Q_updator(const QLearningOptions& opt, const scalar& qsa, const scalar& qsa_prim, const scalar& reward, const size_t&)
{ return (1 - opt.alpha) * qsa + opt.alpha * (reward + opt.gamma * qsa_prim); }

po::variables_map process_args(int argc, char** argv) {
    po::options_description desc("Allowed options");
    auto prob_checker = [](char const * const opt_name) {
        return [opt_name](const scalar& v) {
            if(v < 0 || v > 1)
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, std::to_string(v));
        };
    };
    auto action_checker = [](char const * const opt_name) {
        return [opt_name](const string& v) {
            unordered_map<string, void*> mp = {{"greedy", nullptr}, {"boltzmann", nullptr}};
            if(!mp.count(boost::to_lower_copy(v)))
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, v);
        };
    };
    auto fci_checker = [](char const * const opt_name) {
        return [opt_name](const string& v) {
            unordered_map<string, void*> mp = {{"combiner_k_mean", nullptr}, {"combiner_mean", nullptr}, {"combiner_max", nullptr}};
            if(!mp.count(boost::to_lower_copy(v)))
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, v);
        };
    };
    desc.add_options()
            ("help", "Produces this help message.")
            ("agents", po::value<size_t>()->default_value(1), "The number of agents to run.")
            ("iters", po::value<size_t>()->default_value(20), "The iterations that the program should run itself.")
            ("trials", po::value<size_t>()->default_value(200), "The agents' trials at each program iteration.")
            ("action_picker,a", po::value<string>()->default_value("boltzmann")->notifier(action_checker("action_picker")), "The action picker method, could be [greedy, boltzmann].")
            ("beta,b", po::value<scalar>()->default_value(.01, ".01")->notifier(prob_checker("beta")), "The learning rate(beta) rate, should be in range of [0,1].")
            ("gamma,g", po::value<scalar>()->default_value(.9, ".90")->notifier(prob_checker("gamma")), "The discount rate(gamma) rate, should be in range of [0,1].")
            ("tau,t", po::value<scalar>()->default_value(.4, ".40"), "The temperature rate(gamma) value.")
            ("greedy_explore_rate,g", po::value<scalar>()->default_value(.2, ".20")->notifier(prob_checker("ger")), "The greedy action picker exploration rate, should be in range of [0,1].")
            ("fci_combine_method,f", po::value<string>()->default_value("combiner_k_mean")->notifier(fci_checker("fcm")), "The FCI combinator method, could be [combiner_k_mean, combiner_mean, combiner_max].")
            ("agents_learning_cycle, c", po::value<size_t>()->default_value(5), "The number of agents learning cycle.")
            ("display_defaults", "Displays the default values for inputs")
        ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(po::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      exit(EXIT_FAILURE);
    }
    if (vm.count("help")) { cout << desc << endl; exit(EXIT_SUCCESS); }
    if (vm.count("display_defaults")) {
        foreach_elem(e, vm) {
            if(e.first == "display_defaults") continue;
            auto& v = e.second.value();
            cout << "[" << e.first.c_str() << "] = ";
            if(auto vv = boost::any_cast<string>(&v))
                cout << *vv;
            else if(auto vv = boost::any_cast<size_t>(&v))
                cout << *vv;
            else if(auto vv = boost::any_cast<scalar>(&v))
                cout << *vv;
            else throw runtime_error("This should not happen!!");
            cout << endl << flush;
        }

        exit(EXIT_SUCCESS);
    }
    return vm;
}
