#include "Node.h"

Node::Node(int numActions) : numActions(numActions) {
    regretSum.resize(static_cast<unsigned long>(numActions));
    strategy.resize(static_cast<unsigned long>(numActions));
    strategySum.resize(static_cast<unsigned long>(numActions));
}

vector<DB> Node::getStrategy(DB realizationWeight, uint32_t iteration) {
    for (int i = 0; i < numActions; ++i) {
        strategy[i] = max(regretSum[i], (DB) 0.0);
    }
    DB normalizingSum = 0.0;
    for (auto const &i : strategy) normalizingSum += i;
    if (normalizingSum > 0) {
        for (auto &i : strategy) i /= normalizingSum;
    } else {
        strategy = vector<DB>(static_cast<unsigned long>(numActions), 1.0 / numActions);
    }

    for (int i = 0; i < numActions; ++i) {
        strategySum[i] += iteration * realizationWeight * strategy[i];
    }
    return strategy;
}

vector<DB> Node::getAvgStrategy() const {
    vector<DB> avgStrategy = strategySum;
    DB normalizingSum = 0.0;
    for (auto const &i : avgStrategy) normalizingSum += i;
    if (normalizingSum > 0) {
        for (auto &i : avgStrategy) i /= normalizingSum;
    } else {
        avgStrategy = vector<DB>(static_cast<unsigned long>(numActions), 1.0 / numActions);
    }
    return avgStrategy;
}

string Node::toString() const {
    string res = "[";
    bool isFirst = true;
    for (const auto &i : Node::getAvgStrategy()) {
        if (isFirst) isFirst = false;
        else res += ", ";
        res += to_string(i);
    }
    return res + "]";
}
