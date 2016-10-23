#include "main.helper.hpp"
#include <future>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "inc/fci.hpp"
#include "inc/qcom.hpp"
namespace po = boost::program_options;

void print_refmat(const vector<vector<block>>& refmat) {
    foreach_elem(e, refmat) {
        foreach_elem(k, e)
            cout << k.value() << " ";
        cout <<endl;
    }
}

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

/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int argc, char** argv) {

    auto opt = process_args(argc, argv);

    Maze_QLearning::action_func_t action_picker = boltzmann_action_picker;
    if(boost::to_lower_copy(opt["action_picker"].as<string>()) == "greedy") action_picker = greedy_action_picker;
    ::CONF_RATE_TAU = opt["tau"].as<scalar>();
    ::CONF_RATE_BETA = opt["beta"].as<scalar>();
    ::CONF_ITERATIONS = opt["iters"].as<size_t>();
    ::CONF_RATE_GAMMA = opt["gamma"].as<scalar>();
    ::CONF_TRIAL_MAX = opt["trials"].as<size_t>();
    ::CONF_MULTI_AGENT_COUNT = opt["agents"].as<size_t>();
    ::CONF_RATE_GREEDY_EXPLORE = opt["greedy_explore_rate"].as<scalar>();
    ::CONF_AGENT_LEARNING_CYCLES = opt["agents_learning_cycle"].as<size_t>();


    bool exiting = false;
    vector<future<void>> thread_pool;
    thread_pool.push_back(std::async(std::launch::async, [&exiting]() { while(!exiting) { updateseed(); std::this_thread::sleep_for(std::chrono::milliseconds(100));}} ));

    maze m = create_maze(
                // the size of maze
                {6, 6},
                // the reference size
                3,
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

    future<QLearningResult>* threads = new future<QLearningResult>[::CONF_MULTI_AGENT_COUNT];

    while(::CONF_ITERATIONS--) {

        vector<scalar> avg_moves;
        vector<QLearningResult> results;
        vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};
        qtable_t qtable = Maze_QLearning::init_Qtable(m.width, m.height, action_list);

        foreach_trial(_try) {
            // the result container
            results.clear();
            // make them learn
            foreach_agent(gid)
                threads[gid] =
                    std::async(std::launch::async,
                               execute_agent,
                               m,
                               action_list,
                               qtable,
                               action_picker,
                               Q_updator,
                               ::CONF_AGENT_LEARNING_CYCLES,
                               size_t(gid));
            // fetch the results
            foreach_agent(gid)
                results.push_back(threads[gid].get());
            // calc the average moves
            auto avm = get_avg_hop_single(results);
            // store the calced avg moves for later outputing
            avg_moves.insert(avg_moves.end(), avm.begin(), avm.end());
            // if using single agent
            if(::CONF_MULTI_AGENT_COUNT == 1) {
                // don't combine for single agent
                qtable = results.back()._qtable;
//                print_refmat(results.back()._refmat);
//                cout << endl;
            }
            else
                // combine the intel
                qtable = QCom(results, m.ref_size).combine();
        }

        for(size_t i = 0; i < avg_moves.size(); i++)
            cout << accumulate(avg_moves.begin(), avg_moves.begin() + i + 1, 0.0) / (i + 1) << " ";
        cout <<endl;

        assert(results.size() == ::CONF_MULTI_AGENT_COUNT);

        foreach_agent(gid) {
            break;
            cout << endl << endl << "POLICY FOR AGENT# " << gid << endl << endl;
            print_policy(m, results[gid]._policy);
        }
    }

    exiting = true;
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    delete[] threads;
    exit(0);
}
