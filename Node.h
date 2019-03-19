#include <utility>

#ifndef CFR_NODE_H
#define CFR_NODE_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Node {
public:
    int numActions = 0;
    vector<double> regretSum;
    vector<double> strategy;
    vector<double> strategySum;

    explicit Node(int numActions);

    vector<double> getStrategy(double realizationWeight, uint32_t iteration);

    vector<double> getAvgStrategy() const;

    string toString() const;

    Node() = default;

    ~Node() = default;

private:
};


#endif //CFR_NODE_H
