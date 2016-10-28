#include "inc/stdafx.hpp"

#include <mutex>
#include <unordered_map>

void flag_pass(const string& s) {
    static std::mutex mtx;
    static unordered_map<string, void*> visit;
    mtx.lock();
    if(!visit.count(s)) {
        cerr << s << endl;
        visit.insert({s, nullptr});
    }
    mtx.unlock();
}
