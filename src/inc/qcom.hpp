#ifndef QCOM_H
#define QCOM_H

#include "stdafx.hpp"
#include "rl/qlearning.hpp"


class QCom
{
public:
    const size_t ref_size;
    const vector<QLearningResult> results;
public:
    QCom(const vector<QLearningResult>& results, const size_t& ref_size)
        : ref_size(ref_size), results(results)
    { }
    qtable_t combine() const;
};

#endif // QCOM_H
