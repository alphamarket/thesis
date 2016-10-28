#ifndef MAIN_EXTEND_HPP
#define MAIN_EXTEND_HPP

#include "stdafx.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

po::variables_map process_args(int argc, char** argv);

#endif // MAIN_EXTEND_HPP
