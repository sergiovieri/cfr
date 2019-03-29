#ifndef CFR_POKERSTATE_H
#define CFR_POKERSTATE_H

#define DB long double

#include <array>
#include <string>
#include "PokerHand.h"

using namespace std;

class PokerState {
public:
    PokerState() = default;

    ~PokerState() = default;

    bool isTerminal();

    pair<DB, DB> getTerminalValue();

    uint8_t getCurrentPlayer() const;

    int getNumActions();

    PokerState getNextState(uint8_t action);

    void swapPlayers();

    PokerHand pokerHand;

    array<string, 4> histories;
    pair<int, int> pot = {10, 20};
    uint8_t currentPlayer = 0;
    uint8_t round = 0;
    pair<uint8_t, uint8_t> playerRaises = {0, 0};
    uint8_t numRaises = 1;

    string &getCurrentRoundHistory();

    int getRaiseAmount();
};


#endif //CFR_POKERSTATE_H
