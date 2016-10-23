#ifndef QCOM_FCI_GPP
#define QCOM_FCI_GPP

#include "qcom.hpp"
#include "fci.hpp"
#include "rl/qlearning.hpp"

vector<scalar> operator-(const vector<scalar>& s, const long& d) {
    vector<scalar> out = s;
    for(size_t j = 0; j < s.size(); j++) out[j] -= d;
    return out;
}
vector<scalar> operator-(const long& d, const vector<scalar>& s) {
    vector<scalar> out = s;
    for(size_t j = 0; j < s.size(); j++) out[j] = d - out[j];
    return out;
}

vector<scalar> operator/(const vector<scalar>& s, const long& d) {
    vector<scalar> out = s;
    for(size_t j = 0; j < s.size(); j++) out[j] /= d;
    return out;
}

refmat_t operator/(const refmat_t& r, const long& d) {
    refmat_t out = r;
    for(size_t i = 0; i < r.size(); i++) out[i] = out[i] / d;
    return out;
}
refmat_t operator+(const refmat_t& r, const refmat_t& s) {
    refmat_t out = r;
    for(size_t i = 0; i < r.size(); i++) {
        for(size_t j = 0; j < r[i].size(); j++) {
            out[i][j] += s[i][j];
        }
    }
    return out;
}

template<typename _out>
vector<vector<_out>> normalize_2d(const vector<vector<scalar>>& v){
    size_t sum = 0;
    foreach_elem(k, v) sum = accumulate(k.begin(), k.end(), 0.0);
    vector<vector<_out>> out = v / sum;
    return out;
}

template<typename _out>
vector<vector<vector<_out>>> normalize_3d(const vector<vector<vector<scalar>>>& l){
    size_t sum = 0;
    vector<vector<vector<_out>>> out;
    foreach_elem(v, l) foreach_elem(k, v) sum = accumulate(k.begin(), k.end(), 0.0);
    for(size_t k = 0; k < l.size(); k++)
    { out.push_back(l[k] / sum); }
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
                    vector<scalar> ref = (1 - evals[ref_x][ref_y] / accumulate(evals[ref_x][ref_y].begin(),  evals[ref_x][ref_y].end(), 0.0));
                    assert(qvl.size() == ref.size());
                    out[x][y][k] = fci::combine(qvl, ref, ::CONF_FCI_COMBINER);
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
        refmat_t avg = e._refmat.front() / scalar(e._refmat.size());
        for(size_t k = 1; k < e._refmat.size(); k++) avg = avg + (e._refmat[k] / scalar(e._refmat.size()));
        norms.push_back(avg);
    }
    return merge(trans_4d(qtables), trans_3d(norms), qcom->ref_size);
}

#endif // QCOM_FCI_GPP
