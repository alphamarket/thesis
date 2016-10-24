#include "fci.hpp"
#include <limits>

struct fci_choquet_package {
    scalar
        _in,
        _val;
    long
        _index;
};

vector<size_t> fci::range_vector(const packages_t& packages, long i, size_t plus, long j) {
    vector<size_t> indexes;
    assert(size_t(i) < packages.size() && i < j);
    while(size_t(i) < packages.size() && i < j) indexes.push_back(packages[i++]._index + plus);
    sort(indexes.begin(), indexes.end());
    return indexes;
}

string fci::range_string(const packages_t& packages, long i, size_t plus, long j) {
    string out = "";
    auto vec = range_vector(packages, i, plus, j);
    foreach_elem(e, vec) out += to_string(e) + ",";
    return out.substr(0, out.length()-1);
}

scalar fci::master_combiner(const packages_t& packages, const fci_combiner_func_t &slave_combiner, const size_t &i) {
    if(i == 1) return 1.;
    else if(i >= packages.size()) return 0.;
    else return slave_combiner(packages, i);
}

scalar fci::combiner_max(const packages_t& packages, const size_t& i) {
    scalar _max = -INFINITY;
    auto vec = fci::range_vector(packages, i, 1);
    for(size_t j = 0, k = 1; j < vec.size(); j++, k++) { _max = max(_max, packages[vec[j]]._val); }
    return _max;
}

scalar fci::combiner_mean(const packages_t& packages, const size_t& i) {
    scalar out = 0;
    auto vec = fci::range_vector(packages, i, 1);
    for(size_t j = 0, k = 1; j < vec.size(); j++, k++) { out += packages[vec[j]]._val; }
    return out / vec.size();
}

scalar fci::combiner_k_mean(const packages_t& packages, const size_t& i) {
    size_t div = 0;
    scalar out = 0;
    vector<scalar> mus;
    vector<size_t> vec = fci::range_vector(packages, i, 1);
    foreach_elem(v, vec) mus.insert(std::upper_bound(mus.begin(), mus.end(), packages[v]._val), packages[v]._val);
    for(size_t j = 0, k = 1; j < mus.size(); j++, k++)  { out += k * mus[j]; div += k; }
    // make it less averaged!
    return out / (div - (div == 1 ? 0 : 1));
}

scalar fci::combine(const vector<scalar>& ins, const vector<scalar>& vals,  const fci_combiner_func_t &combiner_func) {
    scalar out = 0;
    assert(ins.size() == vals.size());
    // create choquet packages
    packages_t packages;
    // fill the packages
    for(long i = 0; size_t(i) < ins.size(); i++) packages.push_back({ins[i], vals[i], i});
    // sort ascending
    sort(packages.begin(), packages.end(), [](auto i, auto j) { return i._in < j._in; });
    // insert the init value
    packages.insert(packages.begin(), {0, 0, -1});
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
    // if the \empty condition of \mu does not exists? add it
    if(!vals.count("")) vals.insert({"", 0});
    // if the \all condition of \mu does not exists? add it
    if(!(vals.count(fci::range_string(packages, 1, 1)) || vals.count(fci::range_string(packages, 0, 0, packages.size()-1)))) vals.insert({fci::range_string(packages, 1, 1), 1});
    // ensure the default value are set correctly
    if(vals[""] != 0) vals[""] = 0;
    if(vals.count(fci::range_string(packages, 1, 1))) {
        if(vals[fci::range_string(packages, 1, 1)] != 1) vals[fci::range_string(packages, 1, 1)] = 1;
    } else if(vals.count(fci::range_string(packages, 0, 0, packages.size()-1))) {
        if(vals[fci::range_string(packages, 0, 0, packages.size()-1)] != 1) vals[fci::range_string(packages, 0, 0, packages.size()-1)] = 1;
    } else throw runtime_error("This should no happend!!");
    // foreach packages
    for(long i = 1; size_t(i) < packages.size(); i++) {
        scalar factor = 0;
        // get the factor with zero-based or one-based input tolerance
        if(vals.count(fci::range_string(packages, i, 1)))
           factor = vals[fci::range_string(packages, i, 1)];
        else if(vals.count(fci::range_string(packages, i)))
            factor = vals[fci::range_string(packages, i)];
        else throw runtime_error("The key value {"+fci::range_string(packages, i)+"} is not defined!!");
        // [x(i) - x(i-1)] * \mu{\tau(i...n)}
        out += (packages[i]._in - packages[i - 1]._in) * factor;
    }
    return out;
}

void fci::test() {
    vector<vector<scalar>> grades = {
        {18, 16, 10},
        {10, 12, 18},
        {14, 15, 15}
    };
    vector<scalar> kk = {.45, .45, .3};
    unordered_map<string, scalar> k = {
        {"1", kk[0]},{"2", kk[1]},{"3", kk[2]},
        {"1,3", .9},{"2,3", .9},{"1,2",.5}
    };
    size_t i = 0;
    foreach_elem(g, grades) {
        cout << "Student#" << i << " = " << fci::combine(g, kk, fci::combiner_k_mean) << endl;
    }
}
