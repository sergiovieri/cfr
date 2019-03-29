#ifndef CFR_ABSTRACTOREHS2_H
#define CFR_ABSTRACTOREHS2_H

#include <cassert>
#include <string>

using namespace std;

class AbstractorEHS2 {
public:
    AbstractorEHS2() = delete;

    static constexpr array<pair<double, double>, 20> flopBucket{
            {
                    {0.101456, 0.188832},
                    {0.123186, 0.231488},
                    {0.141547, 0.267828},
                    {0.160202, 0.303546},
                    {0.178681, 0.337738},
                    {0.199609, 0.365623},
                    {0.219483, 0.393674},
                    {0.239416, 0.42229},
                    {0.261036, 0.450845},
                    {0.286615, 0.475709},
                    {0.316226, 0.503452},
                    {0.345526, 0.536103},
                    {0.373256, 0.564127},
                    {0.410473, 0.594363},
                    {0.454793, 0.631974},
                    {0.505057, 0.672165},
                    {0.567317, 0.714859},
                    {0.647643, 0.767578},
                    {0.740123, 0.825548},
                    {1, 0.911215}
            }};

    static constexpr array<pair<double, double>, 20> turnBucket{
            {
                    {0.0526015, 0.123803},
                    {0.0749334, 0.186605},
                    {0.0997095, 0.234695},
                    {0.120278, 0.270037},
                    {0.141065, 0.304293},
                    {0.165198, 0.336737},
                    {0.189298, 0.375176},
                    {0.213662, 0.410738},
                    {0.240839, 0.433663},
                    {0.272517, 0.471454},
                    {0.299626, 0.508169},
                    {0.335838, 0.537275},
                    {0.381585, 0.575516},
                    {0.430142, 0.620026},
                    {0.495838, 0.665964},
                    {0.563351, 0.720575},
                    {0.641879, 0.768884},
                    {0.720254, 0.823518},
                    {0.828802, 0.87186},
                    {1, 0.947552}
            }
    };

    static string rankToChar(int rank) {
        if (rank < 8) return string(1, '2' + rank);
        if (rank == 8) return "t";
        if (rank == 9) return "j";
        if (rank == 10) return "q";
        if (rank == 11) return "k";
        if (rank == 12) return "a";
        assert(false);
        __builtin_unreachable();
    }

    static string getFlopInfo(const PokerState &state) {
        double ehs = state.pokerHand.val[PokerHand::HS][state.round - 1][state.getCurrentPlayer()];
        double ehs2 = state.pokerHand.val[PokerHand::HS2][state.round - 1][state.getCurrentPlayer()];
        for (int bucket = 0; bucket < 20; ++bucket) {
            if (ehs2 <= flopBucket[bucket].first) {
                if (ehs <= flopBucket[bucket].second) {
                    return to_string(bucket) + to_string(0);
                } else {
                    return to_string(bucket) + to_string(1);
                }
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    static string getTurnInfo(const PokerState &state) {
        double ehs = state.pokerHand.val[PokerHand::HS][state.round - 1][state.getCurrentPlayer()];
        double ehs2 = state.pokerHand.val[PokerHand::HS2][state.round - 1][state.getCurrentPlayer()];
        for (int bucket = 0; bucket < 20; ++bucket) {
            if (ehs2 <= turnBucket[bucket].first) {
                if (ehs <= turnBucket[bucket].second) {
                    return to_string(bucket) + to_string(0);
                } else {
                    return to_string(bucket) + to_string(1);
                }
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    static string getRiverInfo(const PokerState &state) {
        double ehs = state.pokerHand.val[PokerHand::HS][state.round - 1][state.getCurrentPlayer()];
        return to_string((int) floor((long double) ehs * 40.0));
    }

    static string getWPInfo(const PokerState &state) {
        if (state.round == 1) {
            return getFlopInfo(state);
        } else if (state.round == 2) {
            return getTurnInfo(state);
        } else if (state.round == 3) {
            return getRiverInfo(state);
        }
        assert(false);
        __builtin_unreachable();
    }

    static string getCardInfo(const PokerState &state) {
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


#endif //CFR_ABSTRACTOREHS2_H
