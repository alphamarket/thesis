#include "fci.hpp"
#include <limits>

struct fci::choquet_package {
    scalar
        _in,
        _val;
    long
        _index;
};

scalar fci::master_combiner(const packages_t& packages, const combiner_func_t &slave_combiner, const size_t &i) {
    if(i == 1) return 1.;
    else if(i >= packages.size()) return 0.;
    else return slave_combiner(packages, i);
}

scalar fci::combiner_max(const packages_t& packages, const size_t& i) {
    scalar _max = -INFINITY;
    for(size_t j = i, k = 1; j < packages.size(); j++, k++) { _max = max(_max, packages[j]._val); }
    return _max;
}

scalar fci::combiner_mean(const packages_t& packages, const size_t& i) {
    size_t div = 0;
    scalar out = 0;
    for(size_t j = i, k = 1; j < packages.size(); j++, k++) { out += packages[j]._val; div += 1; }
    return out / div;
}

scalar fci::combiner_k_mean(const packages_t& packages, const size_t& i) {
    size_t div = 0;
    scalar out = 0;
    // assuming that the package is sorted
    for(size_t j = i, k = 1; j < packages.size(); j++, k++) { out += k * packages[j]._val; div += k; }
    return out / div;
}

scalar fci::combine(const vector<scalar>& ins, const vector<scalar>& vals,  const combiner_func_t &combiner_func) {
    scalar out = 0;
    assert(ins.size() == vals.size());
    // create choquet packages
    packages_t packages;
    // fill the packages
    for(long i = 0; size_t(i) < ins.size(); i++) packages.push_back({ins[i], vals[i], i});
    // sort ascending
    sort(packages.begin(), packages.end(), [](auto i, auto j) { return i._val < j._val; });
    // insert the init value
    packages.insert(packages.begin(), {0, 0, 0});
    // foreach packages
    for(long i = 1; size_t(i) < packages.size(); i++)
        // [x(i) - x(i-1)] * tau(i...n)
        out += (packages[i]._in - packages[i - 1]._in) * fci::master_combiner(packages, combiner_func, i);
    return out;
}

scalar fci::combine(const vector<scalar>& ins, unordered_map<string,scalar> vals) {
    scalar out = 0;
    // make sure there anything to work upon
    assert(ins.size() && vals.size());
    // create choquet packages
    packages_t packages;
    // fill the packages
    for(long i = 0; size_t(i) < ins.size(); i++) packages.push_back({ins[i], 0, i});
    // sort ascending
    sort(packages.begin(), packages.end(), [](auto i, auto j) { return i._in < j._in; });
    // insert the init value
    packages.insert(packages.begin(), {0, 0, -1});
    /**
     * @brief creates choquent \tua range string
     */
    auto range_string = [&packages](long i, size_t plus = 0, long j = std::numeric_limits<long>::max()) -> string {
        string out = "";
        vector<size_t> indexes;
        assert(size_t(i) < packages.size() && i < j);
        while(size_t(i) < packages.size() && i < j) indexes.push_back(packages[i++]._index + plus);
        sort(indexes.begin(), indexes.end());
        foreach_elem(e, indexes) out += to_string(e) + ",";
        return out.substr(0, out.length()-1);
    };
    // if the \empty condition of \mu does not exists? add it
    if(!vals.count("")) vals.insert({"", 0});
    // if the \all condition of \mu does not exists? add it
    if(!(vals.count(range_string(1, 1)) || vals.count(range_string(0, 0, packages.size()-1)))) vals.insert({range_string(1, 1), 1});
    // ensure the default value are set correctly
    if(vals[""] != 0) vals[""] = 0;
    if(vals.count(range_string(1, 1))) {
        if(vals[range_string(1, 1)] != 1) vals[range_string(1, 1)] = 1;
    } else if(vals.count(range_string(0, 0, packages.size()-1))) {
        if(vals[range_string(0, 0, packages.size()-1)] != 1) vals[range_string(0, 0, packages.size()-1)] = 1;
    } else throw runtime_error("This should no happend!!");
    // foreach packages
    for(long i = 1; size_t(i) < packages.size(); i++) {
        scalar factor = 0;
        // get the factor with zero-based or one-based input tolerance
        if(vals.count(range_string(i, 1)))
           factor = vals[range_string(i, 1)];
        else if(vals.count(range_string(i)))
            factor = vals[range_string(i)];
        else throw runtime_error("The key value {"+range_string(i)+"} is not defined!!");
        // [x(i) - x(i-1)] * \mu{\tau(i...n)}
        out += (packages[i]._in - packages[i - 1]._in) * factor;
    }
    return out;
}
