#include <utility>

#include <iostream>

#include "PokerState.h"

omp::EquityCalculator PokerState::eq;

mt19937 PokerState::randomGenerator = mt19937(random_device()());

bool PokerState::isTerminal() {
    if (round == 4) return true;
    const auto &history = getCurrentRoundHistory();
    if (history.empty()) return false;
    return history.back() == 'f';
}

uint8_t PokerState::getCurrentPlayer() {
    return currentPlayer;
}

int PokerState::getNumActions() {
    const auto player = getCurrentPlayer();
    if ((player == 0 ? playerRaises.first : playerRaises.second) == 4 || numRaises == 4) return 2;
    return 3;
}

PokerState PokerState::getNextState(uint8_t action) {
    PokerState nextState = *this;
    const uint8_t player = getCurrentPlayer();
    const auto opponent = static_cast<const uint8_t>(1 - player);
    const auto currentRound = round;
    nextState.currentPlayer = opponent;

    if (action == 0) {
        nextState.histories[currentRound] += 'f';
    } else if (action == 1) {
        nextState.histories[currentRound] += 'c';
        if (player == 0) nextState.pot.first = nextState.pot.second;
        else nextState.pot.second = nextState.pot.first;
        if (nextState.histories[currentRound].length() > 1) {
            nextState.round += 1;
            nextState.numRaises = 0;
            nextState.currentPlayer = 0;
        }
    } else {
        nextState.histories[currentRound] += 'r';
        if (player == 0) {
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

string PokerState::getInfoSet() {
    return to_string(round) + ": " +
           to_string(getCurrentPlayer() == 0 ? winProbs[round].first : winProbs[round].second) +
           histories[0] + ';' + histories[1] + ';' + histories[2] + ';' + histories[3];
}

pair<DB, DB> PokerState::getTerminalValue() {
    if (round == 4) {
        if (win == 1) return {pot.second, -pot.second};
        else if (win == -1) return {-pot.first, pot.first};
        else return {0.0, 0.0};
    }
    const auto player = getCurrentPlayer();
    if (player == 0) return {pot.second, -pot.second};
    else return {-pot.first, pot.first};
}

PokerState::PokerState() {
    generateCards();
}

uint64_t PokerState::pickCard(uint64_t &used) {
    uniform_int_distribution<> dist(0, 51);
    while (true) {
        int x = dist(randomGenerator);
        if (used & (1LU << x)) continue;
        used |= (1LU << x);
        return 1LU << x;
    }
}

uint64_t PokerState::pickCards(int num, uint64_t &used) {
    uint64_t res = 0;
    while (num--) {
        res |= pickCard(used);
    }
    return res;
}

void PokerState::generateCards() {
    uint64_t used = 0;
    holeCards = toCardRange(pickCards(2, used));
    oppCards = toCardRange(pickCards(2, used));
    roundCards[0] = 0;
    roundCards[1] = pickCards(3, used);
    roundCards[2] = roundCards[1] + pickCards(1, used);
    roundCards[3] = roundCards[2] + pickCards(1, used);

    for (int i = 0; i < 4; ++i) {
        winProbs[i] = getWinProb(roundCards[i]);
    }

    omp::Hand commHand = omp::Hand::empty();

    for (unsigned int i = 0; i < 52; ++i) {
        if (roundCards[3] & (1LU << i)) commHand += omp::Hand(i);
    }

    auto h1 = omp::Hand(holeCards.combinations()[0]);
    auto h2 = omp::Hand(oppCards.combinations()[0]);
    auto s1 = he.evaluate(commHand + h1);
    auto s2 = he.evaluate(commHand + h2);

    if (s1 == s2) {
        win = 0;
    } else if (s1 > s2) {
        win = 1;
    } else {
        win = -1;
    }

//    bool valid = true;
//    if (winProbs[3].first < winProbs[3].second) {
//        if (win != -1) valid = false;
//    } else if (winProbs[3].first == winProbs[3].second) {
//        if (win != 0) valid = false;
//    } else {
//        if (win != 1) valid = false;
//    }
//    if (!valid) {
//        for (int i = 0; i < 4; ++i) {
//            cout << "(" << (int)winProbs[i].first << ", " << (int)winProbs[i].second << ")\n";
//        }
//
//        cout << s1 << " " << s2 << '\n';
//    }

}

omp::CardRange PokerState::toCardRange(uint64_t cards) {
    array<uint8_t, 2> combo = {255, 255};
    for (uint8_t i = 0; i < 52; ++i) {
        if (cards & (1LU << i)) {
            if (combo[0] == 255) combo[0] = i;
            else combo[1] = i;
        }
    }
    return omp::CardRange({combo});
}

pair<uint8_t, uint8_t> PokerState::getWinProb(uint64_t roundCard) {
    return make_pair(getWinProb(holeCards, roundCard), getWinProb(oppCards, roundCard));
}

uint8_t PokerState::getWinProb(omp::CardRange card, uint64_t roundCard) {
    PokerState::eq.setTimeLimit(0.001);
    vector<omp::CardRange> cr = {std::move(card), omp::CardRange("random")};
    PokerState::eq.start(cr, roundCard);
    PokerState::eq.wait();
    return static_cast<uint8_t>(floor(eq.getResults().equity[0] * 20));
}

string &PokerState::getCurrentRoundHistory() {
    return histories[round];
}

int PokerState::getRaiseAmount() {
//    return 10;
    if (round < 2) return 20;
    else return 40;
}
