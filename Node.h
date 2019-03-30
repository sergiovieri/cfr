#include <utility>

#ifndef CFR_NODE_H
#define CFR_NODE_H

#define DB long double

#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Node {
public:
    int numActions = 0;
    vector<DB> regretSum;
    vector<DB> strategy;
    vector<DB> strategySum;

    explicit Node(int numActions);

    vector<DB> getStrategy(DB realizationWeight, uint32_t iteration);

    vector<DB> getAvgStrategy() const;

    string toString() const;

    Node() = default;

    ~Node() = default;

private:
};


#endif //CFR_NODE_H
