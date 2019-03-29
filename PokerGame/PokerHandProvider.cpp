#include "PokerHandProvider.h"
#include <cassert>

ifstream PokerHandProvider::ifs = ifstream(PRECOM_FILE);

PokerHand PokerHandProvider::getPokerHand() {
    string s;
    ifs >> s;
    if (s.empty()) ifs = ifstream(PRECOM_FILE);
    ifs >> s;
    assert(s.size());
    return PokerHand(s);
}
