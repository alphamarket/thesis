#ifndef QCOM_H
#define QCOM_H

#include "stdafx.hpp"
#include "rl/qlearning.hpp"


class QCom
{
    const size_t _ref_size;
    const vector<QLearningResult> _results;
public:
    QCom(const vector<QLearningResult>& results, const size_t& ref_size)
        : _ref_size(ref_size), _results(results)
    { }
    qtable_t combine() const;
};

#endif // QCOM_H
