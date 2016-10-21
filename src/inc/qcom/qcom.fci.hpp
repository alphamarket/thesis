#ifndef QCOM_FCI_GPP
#define QCOM_FCI_GPP

#include "qcom.hpp"
#include "rl/qlearning.hpp"

template<typename _out>
vector<vector<_out>> normalize_2d(const vector<vector<block>>& v){
    size_t sum = 0;
    vector<vector<_out>> out;
    foreach_elem(k, v) foreach_elem(e, k) sum += e.value();
    for(size_t i = 0; i < v.size(); i++) {
        out.push_back(vector<scalar>());
        for(size_t j = 0; j < v[i].size(); j++) {
            out.back().push_back(floor(v[i][j].value()) / sum);
        }
    }
    return out;
}

template<typename _out>
vector<vector<vector<_out>>> normalize_3d(const vector<vector<vector<block>>>& l){
    size_t sum = 0;
    vector<vector<vector<_out>>> out;
    foreach_elem(v, l) foreach_elem(k, v) foreach_elem(e, k) sum += e.value();
    for(size_t k = 0; k < l.size(); k++) {
        out.push_back(vector<vector<scalar>>());
        for(size_t i = 0; i < l[k].size(); i++) {
            out.back().push_back(vector<scalar>());
            for(size_t j = 0; j < l[k][i].size(); j++) {
                out[k][i].push_back(floor(l[k][i][j].value()) / sum);
            }
        }
    }
    return out;
}
template<typename T>
void ensure_matrix(const vector<vector<vector<T>>>& _refmat) {
    if(!_refmat.size()) return;
    size_t i = _refmat.front().size();
    foreach_elem(e, _refmat) assert(e.size() == i);
    if(i == 0) return;
    i = _refmat.front().front().size();
    foreach_elem(e, _refmat)
        foreach_elem(l, e)
            assert(l.size() == i);

}
template<typename T = scalar>
vector<vector<vector<T>>> create_matrix_3d(const vector<size_t>& sizes) {
    assert(sizes.size() == 3);
    vector<vector<vector<T>>> out;
    for(size_t state_x = 0; state_x < sizes[0]; state_x++) {
        out.push_back(vector<vector<T>>());
        out.back().reserve(sizes[1]);
        for(size_t state_y = 0; state_y < sizes[1]; state_y++) {
            out.back().push_back(vector<T>());
            out.back().reserve(sizes[2]);
            for(size_t state_z = 0; state_z < sizes[2]; state_z++) {
                out.back().back().push_back(T());
            }
        }
    }
    return out;
}

qtable_t merge(const vector<qtable_t>& qtables, const vector<vector<vector<scalar>>>& evals, const size_t& _ref_size) {
    // the choquet integral package
    struct choquet_package { scalar _qvl, _ref; size_t _index; };
    // make sure the number of agents in both are same
    assert(qtables[0][0][0].size() == evals[0][0].size());
    // ensure the sub-sizes
    ensure_matrix(evals);
    foreach_elem(q, qtables) ensure_matrix(q);
    // create the output qtable
    qtable_t out = create_matrix_3d({qtables.size(), qtables[0].size(), qtables[0][0].size()});
    // for each x row
    for(size_t x = 0; x < qtables.size(); x++) {
        // for each y column
        for(size_t y = 0; y < qtables[0].size(); y++) {
            // for each action
            for(size_t k = 0; k < qtables[0][0].size(); k++) {
                // for each agent
                for(size_t a = 0; a < qtables[0][0][0].size(); a++) {
                    size_t
                        ref_x = x / _ref_size,
                        ref_y = y / _ref_size;
                    vector<scalar> qvl = qtables[x][y][k];
                    vector<scalar> ref = evals[ref_x][ref_y];
                    assert(qvl.size() == ref.size());
                    vector<choquet_package> packages = {{0, 0, 0}};
                    for(size_t i = 0; i < qvl.size(); i++) packages.push_back({qvl[i], ref[i], i});

                    sort(packages.begin(), packages.end(), [](auto i, auto j) { return i._ref < j._ref; });

                    auto __unused mu_max_min = [](const vector<choquet_package>& packages, const size_t& i) -> scalar {
                        size_t div = 0;
                        scalar out = 0;
                        for(size_t j = i, k = 1; j < packages.size(); j++, k++) { out += k * packages[j]._ref; div += k; }
                        return out / div;
                    };
                    auto __unused mu_mean = [](const vector<choquet_package>& packages, const size_t& i) -> scalar {
                        size_t div = 0;
                        scalar out = 0;
                        for(size_t j = i, k = 1; j < packages.size(); j++, k++) { out += packages[j]._ref; div += 1; }
                        return out / div;
                    };
                    auto __unused mu_max = [](const vector<choquet_package>& packages, const size_t& i) -> scalar {
                        scalar _max = -INFINITY;
                        for(size_t j = i, k = 1; j < packages.size(); j++, k++) { _max = max(packages[j]._ref); }
                        return max;
                    };

                    auto mu = [&packages](const size_t& i, function<scalar(const vector<choquet_package>&, const size_t&)> mu_handler) -> scalar {
                        if(i == 1) return 1.;
                        else if(i >= packages.size()) return 0.;
                        else return mu_handler(packages, i);
                    };

                    out[x][y][k] = 0;
                    for(size_t i = 1; i < packages.size(); i++) { out[x][y][k] += (packages[i]._qvl - packages[i - 1]._qvl) * mu(i, mu_max); }
                }
            }
        }
    }
    return out;
}
template<typename T>
vector<vector<vector<vector<T>>>> trans_4d(const vector<vector<vector<vector<T>>>>& mat) {
    foreach_elem(q, mat) ensure_matrix(q);
    vector<vector<vector<vector<T>>>> out;
    for(size_t i = 0; i < mat[0].size(); i++) out.push_back(create_matrix_3d<T>({ mat[0][0].size(), mat[0][0][0].size(), mat.size()}));
    // for each agent
    for(size_t i = 0; i < mat.size(); i++)
        // for each x row
        for(size_t j = 0; j < mat[0].size(); j++)
            // for each y column
            for(size_t k = 0; k < mat[0][0].size(); k++)
                // for each action
                for(size_t l = 0; l < mat[0][0][0].size(); l++)
                    out[j][k][l][i] = mat[i][j][k][l];
    return out;
}

template<typename T>
vector<vector<vector<T>>> trans_3d(const vector<vector<vector<T>>>& mat) {
    ensure_matrix(mat);
    vector<vector<vector<T>>> out = create_matrix_3d<T>({ mat[0].size(), mat[0][0].size(), mat.size() });
    // for each agent
    for(size_t i = 0; i < mat.size(); i++)
        // for each x row
        for(size_t j = 0; j < mat[0].size(); j++)
            // for each y column
            for(size_t k = 0; k < mat[0][0].size(); k++)
                out[j][k][i] = mat[i][j][k];
    return out;
}

qtable_t fci_execute(const QCom* const qcom) {
    vector<qtable_t> qtables;
    vector<vector<vector<scalar>>> norms;
    foreach_elem(e, qcom->results) {
        qtables.push_back(e._qtable);
        norms.push_back(normalize_2d<scalar>(e._refmat));
    }
    return merge(trans_4d(qtables), trans_3d(norms), qcom->ref_size);
}

#endif // QCOM_FCI_GPP
