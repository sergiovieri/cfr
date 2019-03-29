#include "PokerHand.h"
#include "EHSCalc.h"

PokerHand::PokerHand() {
    holeCards = oppCards = commCards[FLOP] = 0;
    uint64_t usedCards = 0;
    EHSCalc::fillCards(2, holeCards, usedCards);
    EHSCalc::fillCards(2, oppCards, usedCards);
    EHSCalc::fillCards(2, commCards[FLOP], usedCards);

    for (int r = 0; r < nROUND; r++) {
        if (r) commCards[r] = commCards[r - 1];
        EHSCalc::fillCards(3 + r, commCards[r], usedCards);
        for (int p = 0; p < nPLAYER; p++) {
            uint64_t hand = p ? oppCards : holeCards;
            tie(val[HS][r][p], val[HS2][r][p]) = EHSCalc::getWinPercentageFull(hand, commCards[r]);
        }
    }

    winner = EHSCalc::getWinner(holeCards, oppCards, commCards[RIVER]);
}

void PokerHand::swapPlayers() {
    swap(holeCards, oppCards);
    for (int r = 0; r < nROUND; ++r) {
        swap(val[HS][r][0], val[HS][r][1]);
        swap(val[HS2][r][0], val[HS2][r][1]);
    }
    winner *= -1;
}
