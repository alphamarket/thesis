#ifndef QCOM_H
#define QCOM_H

#include "stdafx.hpp"
#include "rl/qlearning.hpp"


class QCom
{
public:
    QCom(const vector<QLearningResult>& __unused results);
    qtable_t combine() const;
};

#endif // QCOM_H
