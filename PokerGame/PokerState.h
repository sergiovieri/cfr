#ifndef CFR_POKERSTATE_H
#define CFR_POKERSTATE_H

#define DB long double

#include <array>
#include <string>
#include "PokerHand.h"
#include "PokerHandProvider.h"

using namespace std;

class PokerState {
public:
    PokerState() = default;

    ~PokerState() = default;

    bool isTerminal() const;

    pair<DB, DB> getTerminalValue() const;

    uint8_t getCurrentPlayer() const;

    int getNumActions() const;

    PokerState getNextState(uint8_t action) const;

    void swapPlayers();

    PokerHand pokerHand = PokerHandProvider::getPokerHand();

    array<string, 4> histories;
    pair<int, int> pot = {10, 20};
    uint8_t currentPlayer = 0;
    uint8_t round = 0;
    pair<uint8_t, uint8_t> playerRaises = {0, 0};
    uint8_t numRaises = 1;

    const string &getCurrentRoundHistory() const;

    int getRaiseAmount() const;

    uint32_t getHistoryAsInt(uint8_t cRound) const;

    uint32_t getHash() const;
};


#endif //CFR_POKERSTATE_H
