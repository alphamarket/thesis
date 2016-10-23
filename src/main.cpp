#include "main.helper.hpp"
#include <future>
#include <chrono>
#include <algorithm>
#include <unordered_map>

#include "inc/fci.hpp"
#include "inc/qcom.hpp"

void print_refmat(const vector<vector<vector<scalar>>>& refmat) {
    foreach_elem(e, refmat) {
        foreach_elem(k, e)
            foreach_elem(s, k)
                cout << s << " ";
        cout <<endl;
    }
}

void recall_refmat(vector<maze::refmat_t>& now, maze::refmat_t& past) {
    for(size_t k = 0; k < now.size(); k++) {
        assert(now[k].size() == past.size());
        for(size_t i = 0; i < now[k].size(); i++) {
            assert(now[k][i].size() == past[i].size());
            for(size_t j = 0; j < past.size(); j++) {
                // if not recalling it? recall it
                if(now[k][i][j] == 0) now[k][i][j] = past[i][j];
                // otherwise memorise it
                else past[i][j] = now[k][i][j];
            }
        }
    }
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

    unordered_map<string, fci_combiner_func_t> combiner = {
        {"max", fci::combiner_max},
        {"mean", fci::combiner_mean},
        {"k_mean", fci::combiner_k_mean}
    };
    if(!combiner.count(opt["fci_combine_method"].as<string>())) throw runtime_error("The `"+opt["fci_combine_method"].as<string>()+"` does not exist!");
    ::CONF_FCI_COMBINER = combiner[opt["fci_combine_method"].as<string>()];

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

    cerr << "The world:" << endl;
    cerr << m << endl;

    future<QLearningResult>* threads = new future<QLearningResult>[::CONF_MULTI_AGENT_COUNT];

    const size_t MAX_ITERATION = ::CONF_ITERATIONS;

    while(::CONF_ITERATIONS--) {

        fprintf(stderr, "\r%.1f%% of all iterations done!", 100 * (1 - scalar(::CONF_ITERATIONS + 1) / MAX_ITERATION));

        vector<scalar> avg_moves;
        vector<qtable_t> qtables;
        vector<QLearningResult> results;
        vector<maze::refmat_t> _recall_refmat;
        vector<action> action_list = {maze::NONE, maze::TOP, maze::RIGHT, maze::DOWN, maze::LEFT};

        foreach_agent(_) {
            qtables.push_back(Maze_QLearning::init_Qtable(m.width, m.height, action_list));
            _recall_refmat.push_back(m.clone().new_refmat().refmat().back());
        }

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
                               qtables[gid],
                               action_picker,
                               Q_updator,
                               ::CONF_AGENT_LEARNING_CYCLES,
                               size_t(gid));
            // fetch the results
            foreach_agent(gid) {
                results.push_back(threads[gid].get());
                assert(results.back()._refmat.size() == ::CONF_AGENT_LEARNING_CYCLES);
                recall_refmat(results.back()._refmat, _recall_refmat[gid]);
            }
            // calc the average moves
            auto avm = get_avg_hop_single(results);
            // store the calced avg moves for later outputing
            avg_moves.insert(avg_moves.end(), avm.begin(), avm.end());
            // if using single agent
            if(::CONF_MULTI_AGENT_COUNT == 1) {
                // don't combine for single agent
                qtables.back() = results.back()._qtable;
            }
            else {
                // combine the intel
                auto qtable = QCom(results, m.ref_size).combine();
                foreach_elem(&e, qtables) e = qtable;
            }
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
    fprintf(stderr, "\r100.0%% of all iterations done!\n");

    exiting = true;
    while(thread_pool.size()) {
        thread_pool.back().get();
        thread_pool.pop_back();
    }
    delete[] threads;
    exit(0);
}
