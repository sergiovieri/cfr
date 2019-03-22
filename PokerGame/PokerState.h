#ifndef CFR_POKERSTATE_H
#define CFR_POKERSTATE_H

#define DB long double

#include <string>
#include <vector>
#include <random>
#include <array>
#include "../omp/CardRange.h"
#include "../omp/EquityCalculator.h"

using namespace std;

class PokerState {
public:
    bool isTerminal();

    uint8_t getCurrentPlayer();

    int getNumActions();

    PokerState getNextState(uint8_t action);

    string getInfoSet();

    pair<DB, DB> getTerminalValue();

    static mt19937 randomGenerator;

    static omp::EquityCalculator eq;
    static omp::HandEvaluator he;

    static uint64_t pickCard(uint64_t &used);

    static uint64_t pickCards(int num, uint64_t &used);

    PokerState();

    ~PokerState() = default;

    void generateCards();

    omp::CardRange holeCards;
    omp::CardRange oppCards;
    array<uint64_t, 4> roundCards;
    array<pair<uint8_t, uint8_t>, 4> winProbs;
    int8_t win;

    array<string, 4> histories;
    pair<int, int> pot = {10, 20};
    uint8_t currentPlayer = 0;
    uint8_t round = 0;
    pair<uint8_t, uint8_t> playerRaises = {0, 0};
    uint8_t numRaises = 1;

    static omp::CardRange toCardRange(uint64_t cards);

    uint8_t getWinProb(omp::CardRange card, uint64_t roundCard);

    pair<uint8_t, uint8_t> getWinProb(uint64_t roundCard);

    string &getCurrentRoundHistory();

    int getRaiseAmount();
};


#endif //CFR_POKERSTATE_H
