#ifndef CFR_CFR_H
#define CFR_CFR_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include "Node.h"
#include "KuhnGame/KuhnState.h"
#include <fstream>

template<typename T>
class CFR {
public:
    pair<double, double> util;
    unordered_map<string, Node> nodeMap;
    uint32_t currentIteration = 0;

    void saveToFile(const string &fileName) {
        ofstream file(fileName);
        for (const auto &i : nodeMap) {
            file << "\"" << i.first << "\": " << i.second.toString() << ",\n";
        }
        file.close();
    }

    void train(uint32_t iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (uint32_t i = 0; i < iterations; ++i) {
            currentIteration = i;
            T state = T();
            pair<double, double> currentUtil = CFR::cfr(state, 1.0, 1.0);
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            if (i % 1000 == 0) {
                auto finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = finish - start;
                start = finish;
                cout << "Iteration " << i << '\n';
                cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
                cout << "Average game value: " << util.first / iterations << ", " << util.second / iterations << '\n';
                saveToFile("latest.txt");
            }
            if (i % 10000 == 0) saveToFile(to_string(i) + ".txt");
        }
        auto finish = std::chrono::high_resolution_clock::now();
        cout << "Average game value: " << util.first / iterations << ", " << util.second / iterations << '\n';
        for (const auto &i : nodeMap) {
            cout << "\"" << i.first << "\": " << i.second.toString() << '\n';
        }
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
    }

    CFR() = default;

    ~CFR() = default;

private:
    pair<double, double> cfr(T state, double p0, double p1) {
//    cout << pState->getInfoSet() << " " << p0 << " " << p1 << '\n';

        int player = state.getCurrentPlayer();

        if (state.isTerminal()) return state.getTerminalValue();

        int numActions = state.getNumActions();
        string infoSet = state.getInfoSet();

        if (!CFR::nodeMap.count(infoSet)) CFR::nodeMap[infoSet] = Node(numActions);

        Node &node = CFR::nodeMap[infoSet];

        auto strategy = node.getStrategy(player ? p1 : p0, currentIteration);
        vector<pair<double, double>> currentUtil(static_cast<unsigned long>(numActions));
        pair<double, double> nodeUtil = {0.0, 0.0};

        for (int i = 0; i < numActions; ++i) {
            T nextState = state.getNextState(i);
            currentUtil[i] = player ?
                             cfr(nextState, p0, p1 * strategy[i]) :
                             cfr(nextState, p0 * strategy[i], p1);
            nodeUtil.first += strategy[i] * currentUtil[i].first;
            nodeUtil.second += strategy[i] * currentUtil[i].second;
        }

        for (int i = 0; i < numActions; ++i) {
            double regret = player ? currentUtil[i].second - nodeUtil.second : currentUtil[i].first - nodeUtil.first;
            node.regretSum[i] += regret * (player ? p0 : p1);
            node.regretSum[i] = max(node.regretSum[i], 0.0);
        }
//    cout << infoSet << " " << nodeUtil.first << " " << nodeUtil.second << '\n';
        return nodeUtil;
    }
};


#endif //CFR_CFR_H
