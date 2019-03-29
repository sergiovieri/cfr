#ifndef CFR_CFR_H
#define CFR_CFR_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include "Abstractors/AbstractorPreflop.h"
#include "Node.h"

template<typename T>
class CFR {
public:
    pair<DB, DB> util;
    unordered_map<string, Node> nodeMap[40000];
    mutex nodeMapMutex[40000];
    mutex stateMutex;
    uint32_t currentIteration = 0;
    chrono::time_point<chrono::high_resolution_clock> start;

    void saveToFile(const string &fileName) {
        ofstream file(fileName);
        for (const auto &i : nodeMap) {
            for (const auto &j : i) {
                file << "\"" << j.first << "\": " << j.second.toString() << ",\n";
            }
        }
        file.close();
    }

    void train_thread(uint32_t iterations) {
        while (true) {
            unique_lock<mutex> lock(stateMutex);
            if (currentIteration >= iterations) break;
            uint32_t iteration = (currentIteration += 2);

            T state = T();
            lock.unlock();

            pair<DB, DB> currentUtil = CFR::cfr(state, 1.0, 1.0);

            lock.lock();
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            lock.unlock();

            state.swapPlayers();
            currentUtil = CFR::cfr(state, 1.0, 1.0);

            lock.lock();
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            if (iteration % 10000 == 0) {
                auto finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<DB> elapsed = finish - start;
                cout << "Iteration " << iteration << '\n';
                cout << "Average time: " << elapsed.count() * 1000.0 / iteration << "ms\n";
                cout << "Average game value: " << util.first / iteration << ", " << util.second / iteration << '\n';
                saveToFile("latest.txt");
            }
            if (iteration % 100000 == 0) saveToFile(to_string(iteration) + ".txt");
            lock.unlock();
        }
    }

    void train(uint32_t iterations) {
        start = std::chrono::high_resolution_clock::now();

        vector<thread> threads;
        const int numThreads = thread::hardware_concurrency();
        cout << "Using " << numThreads << " threads\n";
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(&CFR::train_thread, this, iterations);
        }
        for (auto &thread : threads) thread.join();
        auto finish = std::chrono::high_resolution_clock::now();
        cout << "Average game value: " << util.first / iterations << ", " << util.second / iterations << '\n';
        for (const auto &i : nodeMap) {
            for (const auto &j : i) {
                cout << "\"" << j.first << "\": " << j.second.toString() << '\n';
            }
        }
        std::chrono::duration<DB> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
    }

    CFR() = default;

    ~CFR() = default;

private:
    pair<DB, DB> cfr(const T &state, DB p0, DB p1) {
//    cout << pState->getInfoSet() << " " << p0 << " " << p1 << '\n';

        int player = state.getCurrentPlayer();

        if (state.isTerminal()) return state.getTerminalValue();

        int numActions = state.getNumActions();
        string infoSet = AbstractorPreflop::getInfoSet(state);

        uint32_t hash = state.getHash();

        unordered_map<string, Node> &mp = nodeMap[hash];

        unique_lock<mutex> lock(nodeMapMutex[hash]);
        if (!mp.count(infoSet)) mp[infoSet] = Node(numActions);
        Node &node = mp[infoSet];

        auto strategy = node.getStrategy(player ? p1 : p0, currentIteration);
        lock.unlock();

        vector<pair<DB, DB>> currentUtil(static_cast<unsigned long>(numActions));
        pair<DB, DB> nodeUtil = {0.0, 0.0};

        for (uint8_t i = 0; i < numActions; ++i) {
            T nextState = state.getNextState(i);
            currentUtil[i] = player ?
                             cfr(nextState, p0, p1 * strategy[i]) :
                             cfr(nextState, p0 * strategy[i], p1);
            nodeUtil.first += strategy[i] * currentUtil[i].first;
            nodeUtil.second += strategy[i] * currentUtil[i].second;
        }
        lock.lock();
        for (int i = 0; i < numActions; ++i) {
            DB regret = player ? currentUtil[i].second - nodeUtil.second : currentUtil[i].first -
                                                                           nodeUtil.first;
            node.regretSum[i] += regret * (player ? p0 : p1);
        }
        lock.unlock();
        return nodeUtil;
    }
};


#endif //CFR_CFR_H
