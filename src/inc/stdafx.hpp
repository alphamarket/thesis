#pragma once

#ifdef __unix__
#   define OS_UNIX
#else
#   define OS_WIN
#   warning "The compilation of this source in any other OS than UNIX may cause many errors!"
#endif

#ifdef __GNUC__
#   define __unused __attribute__((unused))
#else
#   define __unused
#endif

#if __cplusplus < 201400
#	errors I require C++14 for this code, so die.
#endif

#include <time.h>
#include <random>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <exception>
#include <algorithm>
#include <functional>
using namespace std;

typedef double scalar;

#define get_rand(min, range)((min) + rand() % (range))
#define frand()             (scalar(rand()) / RAND_MAX)
#ifdef OS_UNIX
#   include <unistd.h>
#   define dynamic_seed     (time(NULL) + rand() + getpid() * 100)
#else
#   define dynamic_seed     (time(NULL) + rand())
#endif
#define updateseed()        srand(dynamic_seed)

#ifdef __GNUC__
#	define __deprecated(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#	define __deprecated(func) __declspec(deprecated) func
#else
#	pragma message("WARNING: You need to implement `__deprecated` for this compiler!")
#	define __deprecated(func) func
#endif

#ifdef OS_UNIX
#   define clear_screen() system("clear")
#else
#   ifdef OS_WIN
#       define clear_screen() system("cls")
#   else
#       define clear_screen()
#   endif
#endif

#define foreach_elem(i, j)     for(auto i : j)
#define foreach_trial(_try)    for(size_t _try = 0; _try < ::CONF_TRIAL_MAX; _try++)
#define foreach_agent(tid)     for(size_t tid = 0; tid < ::CONF_MULTI_AGENT_COUNT; tid++)

