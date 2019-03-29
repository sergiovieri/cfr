#ifndef CFR_BR_H
#define CFR_BR_H

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
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
    unordered_map<string, Node> nodeMap[40000];
    mutex nodeMapMutex[40000];
    mutex stateMutex;
    uint32_t currentIteration = 0;
    uint8_t brPlayer;
    chrono::time_point<chrono::high_resolution_clock> start;

    BR(const unordered_map<string, vector<DB>> &strat, uint8_t _brPlayer) {
        oppStrategy = strat;
        brPlayer = _brPlayer;
    }

    void train_thread(uint32_t iterations) {
        while (true) {
            unique_lock<mutex> lock(stateMutex);
            uint32_t iteration = currentIteration;
            currentIteration += 2;
            if (iteration >= iterations) break;

            T state = T();
            lock.unlock();

            pair<DB, DB> currentUtil = cfr(state, 1.0, 1.0);

            lock.lock();
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            lock.unlock();

            state.swapPlayers();

            currentUtil = cfr(state, 1.0, 1.0);

            lock.lock();
            util.first += currentUtil.first;
            util.second += currentUtil.second;
            lock.unlock();

            iteration += 2;
            if (iteration % 1000 == 0) {
                auto finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<DB> elapsed = finish - start;
                cout << "Iteration " << iteration << '\n';
                cout << "Average time: " << elapsed.count() * 1000.0 / iteration << "ms\n";
                cout << "Average game value: " << util.first / iteration << ", " << util.second / iteration << '\n';
            }
        }
    }

    void train(uint32_t iterations) {
        start = chrono::high_resolution_clock::now();
        vector<thread> threads;
        const int numThreads = thread::hardware_concurrency();
        cout << "Using " << numThreads << " threads\n";
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(&BR::train_thread, this, iterations);
        }
        for (auto &thread : threads) thread.join();
        auto finish = std::chrono::high_resolution_clock::now();
//        for (const auto &i : nodeMap) {
//            cout << "\"" << i.first << "\": " << i.second.toString() << '\n';
//        }
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

    uint32_t getHistoryAsInt(const T &state, uint8_t round) {
        const string &s = state.histories[round];
        if (s.empty()) return 0;
        uint32_t res = 0;
        for (const auto &c : s) if (c == 'r') ++res;
        res *= 2;
        if (s[0] == 'c') ++res;
        return res;
    }

    uint32_t getHash(const T &state) {
        uint32_t res = state.round;
        res *= 10;
        res += getHistoryAsInt(state, 0);
        res *= 10;
        res += getHistoryAsInt(state, 1);
        res *= 10;
        res += getHistoryAsInt(state, 2);
        res *= 10;
        res += getHistoryAsInt(state, 3);
        return res;
    }

    pair<DB, DB> cfr(const T &state, DB p0, DB p1) {

        int player = state.getCurrentPlayer();

        if (state.isTerminal()) return state.getTerminalValue();

        int numActions = state.getNumActions();

        if (player != brPlayer) {
            string infoSet = getStratInfoSet(state);
            auto strategy = oppStrategy[infoSet];
            pair<DB, DB> nodeUtil = {0.0, 0.0};

            for (uint8_t i = 0; i < numActions; ++i) {
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

        uint32_t hash = getHash(state);

        unordered_map<string, Node> &mp = nodeMap[hash];

        unique_lock<mutex> lock(nodeMapMutex[hash]);
        if (!mp.count(infoSet)) mp[infoSet] = Node(numActions);
        Node &node = mp[infoSet];

        auto strategy = node.getStrategy(player ? p1 : p0, currentIteration);
        lock.unlock();
//        fix(strategy);
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
        lock.lock();
        for (int i = 0; i < numActions; ++i) {
            DB regret = player ?
                        currentUtil[i].second - nodeUtil.second :
                        currentUtil[i].first - nodeUtil.first;
            node.regretSum[i] += regret * (player ? p0 : p1);
//            if (node.regretSum[i] < 0) node.regretSum[i] *= 0.9;
//            node.regretSum[i] = max(node.regretSum[i], (DB) -1000.0);
        }
        lock.unlock();
        return nodeUtil;
    }
};


#endif //CFR_BR_H
