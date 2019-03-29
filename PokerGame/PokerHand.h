#ifndef CFR_POKERHAND_H
#define CFR_POKERHAND_H

#include <cstdint>

using namespace std;

class PokerHand {
public:
    static const int nROUND = 3;
    static const int nPLAYER = 2;
    static const int POWER = 2;

    // the whole state
    uint64_t holeCards, oppCards;
    uint64_t commCards[nROUND];
    int winner;

    enum round {
        FLOP, TURN, RIVER
    };
    enum handstr {
        HS, HS2
    };

    double val[POWER][nROUND][nPLAYER];
    // example usage:
    // val[HS2][FLOP][0] => hand strength square, the round is flop, player 1
    // val[HS][FLOP][1]  => hand strength, the round is flop, player 2

    // generate random state, generate HS and HS^2 for flop, turn and river.
    PokerHand();

    void swapPlayers();
};


#endif //CFR_POKERHAND_H
