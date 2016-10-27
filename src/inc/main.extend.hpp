#ifndef MAIN_EXTEND_HPP
#define MAIN_EXTEND_HPP

#include "stdafx.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

po::variables_map process_args(int argc, char** argv) {
    po::options_description desc("Allowed options");
    auto prob_checker = [](char const * const opt_name) {
        return [opt_name](const scalar& v) {
            if(v < 0 || v > 1)
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, std::to_string(v));
        };
    };
    auto fci_checker = [](char const * const opt_name) {
        return [opt_name](const string& v) {
            unordered_map<string, void*> mp = {{"k-mean", nullptr}, {"mean", nullptr}, {"max", nullptr}};
            if(!mp.count(boost::to_lower_copy(v)))
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, v);
        };
    };
    auto method_checker = [](char const * const opt_name) {
        return [opt_name](const string& v) {
            unordered_map<string, void*> mp = {{"fci", nullptr}, {"sep", nullptr}};
            if(!mp.count(boost::to_lower_copy(v)))
                throw po::validation_error(po::validation_error::invalid_option_value, opt_name, v);
        };
    };
    desc.add_options()
            ("help", "Produces this help message.")
            ("agents", po::value<size_t>()->default_value(1), "The number of agents to run.")
            ("iters", po::value<size_t>()->default_value(20), "The iterations that the program should run itself.")
            ("trials", po::value<size_t>()->default_value(200), "The agents' trials at each program iteration.")
            ("cycles", po::value<size_t>()->default_value(5), "The number of agents learning cycle.")
            ("beta", po::value<scalar>()->default_value(.01, ".01")->notifier(prob_checker("beta")), "The learning rate(beta) rate, should be in range of [0,1].")
            ("gamma", po::value<scalar>()->default_value(.9, ".90")->notifier(prob_checker("gamma")), "The discount rate(gamma) rate, should be in range of [0,1].")
            ("tau", po::value<scalar>()->default_value(.4, ".40"), "The temperature rate(gamma) value.")
            ("greedy-explore-rate", po::value<scalar>()->default_value(.2, ".20")->notifier(prob_checker("ger")), "The greedy action picker exploration rate, should be in range of [0,1].")
            ("fci-method", po::value<string>()->default_value("k-mean")->notifier(fci_checker("fci-method")), "The FCI combinator method, could be [k-mean, mean, max].")
            ("method", po::value<string>()->default_value("fci")->notifier(method_checker("method")), "The combiner method, could be [fci, sep].")
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
    return vm;
}


#endif // MAIN_EXTEND_HPP
