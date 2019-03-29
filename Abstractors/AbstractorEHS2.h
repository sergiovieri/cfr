#ifndef CFR_ABSTRACTOREHS2_H
#define CFR_ABSTRACTOREHS2_H

#include <cassert>
#include <string>
#include "../PokerGame/PokerState.h"

using namespace std;

class AbstractorEHS2 {
public:
    AbstractorEHS2() = delete;

    static string getInfoSet(PokerState state);

private:
    static const array<pair<double, double>, 20> flopBucket;

    static const array<pair<double, double>, 20> turnBucket;

    static string rankToChar(int rank);

    static string getFlopInfo(const PokerState &state);

    static string getTurnInfo(const PokerState &state);

    static string getRiverInfo(const PokerState &state);

    static string getWPInfo(const PokerState &state);

    static string getCardInfo(const PokerState &state);
};


#endif //CFR_ABSTRACTOREHS2_H
