#ifndef FCI_HPP
#define FCI_HPP

#include "stdafx.hpp"
#include <unordered_map>

class fci {
public:
    struct choquet_package;
    typedef vector<choquet_package> packages_t;
    typedef function<scalar(const packages_t&, const size_t&)> combiner_func_t;

protected:
    /**
     * @brief master_combiner The master combiner which calls other combiner functions
     * @param packages The choquet packages vector
     * @param slave_combiner The slave combiner
     * @param i The i'th iteration of combination
     * @return The combined value fo i'th iteration
     */
    static scalar master_combiner(const packages_t& packages, const combiner_func_t& slave_combiner, const size_t& i);
    /**
     * @brief fci::range_string Returns string of sorted indices for choquet operations
     * @param packages The choquet packages vector
     * @param i The current index
     * @param plus The add-on value indices
     * @param j The maximum value to iterate to, if not specified the package size will be used
     * @return The ranged string
     */
    static string range_string(const packages_t& packages, long i, size_t plus = 0, long j = std::numeric_limits<long>::max());
    /**
     * @brief fci::range_vector Returns vectors of sorted indices for choquet operations
     * @param packages The choquet packages vector
     * @param i The current index
     * @param plus The add-on value indices
     * @param j The maximum value to iterate to, if not specified the package size will be used
     * @return the ranged vector
     */
    static vector<size_t> range_vector(const packages_t& packages, long i, size_t plus = 0, long j = std::numeric_limits<long>::max());

public:
    /**
     * @brief combiner_max Applies the max operator over values when trying to extract final value for set of inputs
     * @param packages The choquet packages vector
     * @param i The i'th iteration of combination
     * @return The combined value fo i'th iteration
     */
    static scalar combiner_max(const packages_t& packages, const size_t& i);
    /**
     * @brief combiner_mean Applies the mean operator over values when trying to extract final value for set of inputs
     * @param packages The choquet packages vector
     * @param i The i'th iteration of combination
     * @return The combined value fo i'th iteration
     */
    static scalar combiner_mean(const packages_t& packages, const size_t& i);
    /**
     * @brief combiner_k_mean Applies the k-mean operator over values when trying to extract final value for set of inputs
     * @note k-means is the operation of [\sum_{k=K}^{1}{(k-1) * max_{k}(x_i)} / K]
     * @param packages The choquet packages vector
     * @param i The i'th iteration of combination
     * @return The combined value fo i'th iteration
     */
    static scalar combiner_k_mean(const packages_t& packages, const size_t& i);
    /**
     * @brief combine Combines the input with the values
     * @param ins The inputs
     * @param vals The values of inputs
     * @param combiner_func The combiner function
     * @return The combined value
     */
    static scalar combine(const vector<scalar>& ins, const vector<scalar>& vals,  const combiner_func_t& combiner_func);
    /**
     * @brief combine Combines the input with the values
     * @param ins The inputs
     * @param vals The values of input combinations
     * @return The combined value
     */
    static scalar combine(const vector<scalar>& ins, unordered_map<string, scalar> vals);
};

#endif // FCI_HPP
