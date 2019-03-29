#ifndef CFR_ABSTRACTORPREFLOP_H
#define CFR_ABSTRACTORPREFLOP_H

#include <cmath>
#include <string>
#include "../PokerGame/PokerState.h"

using namespace std;

class AbstractorPreflop {
public:
    AbstractorPreflop() = delete;

    static string rankToChar(int rank) {
        if (rank < 8) return string(1, '2' + rank);
        if (rank == 8) return "t";
        if (rank == 9) return "j";
        if (rank == 10) return "q";
        if (rank == 11) return "k";
        if (rank == 12) return "a";
        __builtin_unreachable();
    }

    static string getWPInfo(const PokerState &state) {
        double ehs = state.pokerHand.val[PokerHand::HS][state.round - 1][state.getCurrentPlayer()];
        if (ehs == 1.0) ehs -= 1e-9;
        return to_string((int) floor((long double) ehs * 20.0));
    }

    static string getCardInfo(PokerState state) {
        if (state.round == 0) {
            uint64_t cards;
            if (state.getCurrentPlayer() == 0) cards = state.pokerHand.holeCards;
            else cards = state.pokerHand.oppCards;
            int card1 = -1;
            int card2 = -1;
            for (uint8_t i = 0; i < 52; ++i) {
                if (cards & (1LU << i)) {
                    if (card1 == -1) card1 = i;
                    else card2 = i;
                }
            }
            int rank1 = card1 / 4;
            int rank2 = card2 / 4;
            if (rank1 > rank2) swap(rank1, rank2);
            string info = rankToChar(rank1) + rankToChar(rank2);
            if (card1 % 4 != card2 % 4) info += "o";
            return info;
        }
        return getWPInfo(state);
    }

    static string getInfoSet(PokerState state) {
        return to_string(state.round) + ": " + getCardInfo(state) +
               state.histories[0] + ';' + state.histories[1] + ';' + state.histories[2] + ';' + state.histories[3];
    }
};


#endif //CFR_ABSTRACTORPREFLOP_H
