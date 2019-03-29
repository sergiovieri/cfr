#include "PokerHandProvider.h"
#include <cassert>

ifstream PokerHandProvider::ifs;

PokerHand PokerHandProvider::getPokerHand() {
    string s;
    ifs >> s;
    if (s.empty()) ifs.open(PRECOM_FILE);
    ifs >> s;
    assert(s.size());
    return PokerHand(s);
}
