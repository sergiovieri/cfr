#include "PokerState.h"

bool PokerState::isTerminal() const {
    if (round == 4) return true;
    const auto &history = getCurrentRoundHistory();
    if (history.empty()) return false;
    return history.back() == 'f';
}

pair<DB, DB> PokerState::getTerminalValue() const {
    if (round == 4) {
        if (pokerHand.winner == 1) return {pot.second, -pot.second};
        else if (pokerHand.winner == -1) return {-pot.first, pot.first};
        else return {0.0, 0.0};
    }
    const auto player = getCurrentPlayer();
    if (player == 0) return {pot.second, -pot.second};
    else return {-pot.first, pot.first};
}

uint8_t PokerState::getCurrentPlayer() const {
    return currentPlayer;
}

int PokerState::getNumActions() const {
    const auto player = getCurrentPlayer();
    if ((player == 0 ? playerRaises.first : playerRaises.second) == 4 || numRaises == 4) return 2;
    return 3;
}

PokerState PokerState::getNextState(uint8_t action) const {
    PokerState nextState = *this;
    const auto currentRound = round;
    nextState.currentPlayer ^= 1U;

    if (action == 0) {
        nextState.histories[currentRound] += 'f';
    } else if (action == 1) {
        nextState.histories[currentRound] += 'c';
        if (currentPlayer == 0) nextState.pot.first = nextState.pot.second;
        else nextState.pot.second = nextState.pot.first;
        if (nextState.histories[currentRound].length() > 1) {
            nextState.round += 1;
            nextState.numRaises = 0;
            nextState.currentPlayer = 0;
        }
    } else {
        nextState.histories[currentRound] += 'r';
        if (currentPlayer == 0) {
            nextState.pot.first = nextState.pot.second + getRaiseAmount();
            nextState.playerRaises.first++;
        } else {
            nextState.pot.second = nextState.pot.first + getRaiseAmount();
            nextState.playerRaises.second++;
        }
        nextState.numRaises++;
    }
    return nextState;
}

const string &PokerState::getCurrentRoundHistory() const {
    return histories[round];
}

int PokerState::getRaiseAmount() const {
    if (round < 2) return 20;
    else return 40;
}

void PokerState::swapPlayers() {
    pokerHand.swapPlayers();
}
