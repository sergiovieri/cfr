#ifndef CFR_BR_H
#define CFR_BR_H

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include "Abstractors/AbstractorPreflop.h"
#include "Node.h"
#include "omp/CardRange.h"

using namespace std;

template<typename T>
class BR {
public:
    pair<DB, DB> util;
    unordered_map<string, vector<DB>> oppStrategy;
    unordered_map<string, Node> nodeMap;
    uint32_t currentIteration = 0;
    uint8_t brPlayer;

    BR(const unordered_map<string, vector<DB>> &strat, uint8_t _brPlayer) {
        oppStrategy = strat;
        brPlayer = _brPlayer;
    }

    void train(uint32_t iterations) {
        auto start = chrono::high_resolution_clock::now();
        for (uint32_t i = 0; i < iterations; i += 2) {
            currentIteration = i;
            T state = T();
            pair<DB, DB> currentUtil = cfr(state, 1.0, 1.0);
            util.first += currentUtil.first;
            util.second += currentUtil.second;

            state.swapPlayers();

            currentUtil = cfr(state, 1.0, 1.0);
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            if (i % 1000 == 0) {
                auto finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<DB> elapsed = finish - start;
                start = finish;
                cout << "Iteration " << i << '\n';
                cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
                cout << "Average game value: " << util.first / (i + 2) << ", " << util.second / (i + 2) << '\n';
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        for (const auto &i : nodeMap) {
            cout << "\"" << i.first << "\": " << i.second.toString() << '\n';
        }
        cout << "Average game value: " << util.first / iterations << ", " << util.second / iterations << '\n';
        std::chrono::duration<DB> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
    }

private:

    string getStratInfoSet(T state) {
        return AbstractorPreflop::getInfoSet(state);
    }

    string getBRInfoSet(T state) {
        return AbstractorPreflop::getInfoSet(state);
    }

    void fix(vector<DB> &v) {
        DB mx = -1.0;
        for (DB i : v) mx = max(mx, i);
        DB sum = 0.0;
        for (DB &i : v) {
            if (i == mx) i = 1.0;
            else i = 0.0;
            sum += i;
        }
        for (DB &i : v) i /= sum;
    }

    pair<DB, DB> cfr(T state, DB p0, DB p1) {

        int player = state.getCurrentPlayer();

        if (state.isTerminal()) return state.getTerminalValue();

        int numActions = state.getNumActions();

        if (player != brPlayer) {
            string infoSet = getStratInfoSet(state);
            auto strategy = oppStrategy[infoSet];
            pair<DB, DB> nodeUtil = {0.0, 0.0};

            for (int i = 0; i < numActions; ++i) {
                T nextState = state.getNextState(i);
                pair<DB, DB> currentUtil = player ?
                                           cfr(nextState, p0, p1 * strategy[i]) :
                                           cfr(nextState, p0 * strategy[i], p1);
                nodeUtil.first += strategy[i] * currentUtil.first;
                nodeUtil.second += strategy[i] * currentUtil.second;
            }
            return nodeUtil;
        }

        string infoSet = getBRInfoSet(state);

        if (!nodeMap.count(infoSet)) nodeMap[infoSet] = Node(numActions);

        Node &node = nodeMap[infoSet];

        auto strategy = node.getStrategy(player ? p1 : p0, currentIteration);
        fix(strategy);
        vector<pair<DB, DB>> currentUtil(static_cast<unsigned long>(numActions));
        pair<DB, DB> nodeUtil = {0.0, 0.0};

        for (int i = 0; i < numActions; ++i) {
            T nextState = state.getNextState(i);
            currentUtil[i] = player ?
                             cfr(nextState, p0, p1 * strategy[i]) :
                             cfr(nextState, p0 * strategy[i], p1);
            nodeUtil.first += strategy[i] * currentUtil[i].first;
            nodeUtil.second += strategy[i] * currentUtil[i].second;
        }
        for (int i = 0; i < numActions; ++i) {
            DB regret = player ?
                        currentUtil[i].second - nodeUtil.second :
                        currentUtil[i].first - nodeUtil.first;
            node.regretSum[i] += regret * (player ? p0 : p1);
//            if (node.regretSum[i] < 0) node.regretSum[i] *= 0.9;
//            node.regretSum[i] = max(node.regretSum[i], (DB) -1000.0);
        }
        return nodeUtil;
    }
};


#endif //CFR_BR_H
