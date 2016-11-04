#include "inc/main.extend.hpp"
#include <unordered_map>
#include <boost/algorithm/string.hpp>

po::variables_map process_args(int argc, char** argv) {
    po::options_description desc("Allowed options");
    auto prob_checker = [](char const * const opt_name) {
        return [opt_name](const scalar& v) {
            if(v < 0 || v > 1)
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, std::to_string(v));
        };
    };
    auto avail_options = [](const string& opt_name, const initializer_list<string>& lists) {
        unordered_map<string, void*> mp;
        for(auto l : lists) mp.insert({boost::to_lower_copy(l), nullptr});
        return [opt_name, mp](const string& v) {
            if(!mp.count(boost::to_lower_copy(v)))
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, v);
        };
    };
    desc.add_options()
            ("help", "Produces this help message.")

            ("agents",
                po::value<size_t>()
                    ->default_value(1),
             "The number of agents to run.")

            ("iters",
                po::value<size_t>()
                    ->default_value(20),
            "The iterations that the program should run itself.")

            ("trials",
                po::value<size_t>()
                    ->default_value(200),
             "The agents' trials at each program iteration.")

            ("cycles",
                po::value<size_t>()
                    ->default_value(5),
             "The number of agents learning cycle.")

            ("refmat-grind",
                po::value<size_t>()
                    ->default_value(3),
             "The grind value of fci method.")

            ("beta",
                po::value<scalar>()
                    ->default_value(.01, ".01")
                    ->notifier(prob_checker("beta")),
             "The learning rate(beta) rate, should be in range of [0,1].")

            ("gamma",
                po::value<scalar>()
                    ->default_value(.9, ".90")
                    ->notifier(prob_checker("gamma")),
             "The discount rate(gamma) rate, should be in range of [0,1].")

            ("tau",
                po::value<scalar>()
                    ->default_value(.4, ".40"),
            "The temperature rate(gamma) value.")

            ("epsilon",
                po::value<scalar>()
                    ->default_value(.2, ".20")
                    ->notifier(prob_checker("epsilon")),
             "The greedy action picker exploration rate, should be in range of [0,1].")

            ("refmat-combinator",
                po::value<string>()
                    ->default_value("fci-k-mean")
                    ->notifier(avail_options("refmat-combinator", {"fci-k-mean", "fci-mean", "fci-max", "wsum"})),
             "The REFMAT combinator method, could be [fci-k-mean, fci-mean, fci-max, wsum].")

            ("method",
                po::value<string>()
                    ->default_value("il")
                    ->notifier(avail_options("method", {"refmat", "sep", "il", "sep-refmat"})),
             "The combiner method, could be [refmat, sep, il, sep-refmat].")

            ("env",
                po::value<string>()
                    ->default_value("maze")
                    ->notifier(avail_options("env", {"maze", "prey"})),
             "The combiner method, could be [maze, prey].")

            ("output",
                po::value<string>()
                    ->default_value("avg-moves")
                    ->notifier(avail_options("output", {"avg-moves", "qtable", "qtable-max"})),
             "Prints the different type of information, could be [avg-moves, qtable, qtable-max].")

            ("rl-action-advisor",
                po::value<string>()
                    ->default_value("boltzmann")
                    ->notifier(avail_options("rl-action-advisor", {"boltzmann", "greedy"})),
             "The reinforcement action advisor method, could be [boltzmann, greedy]")
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
    // one agent if the method is IL
    if (vm["method"].as<string>() == "il") vm.at("agents").value() = size_t(1);
    return vm;
}
