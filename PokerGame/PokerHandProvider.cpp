#include "PokerHandProvider.h"
#include <cassert>

ifstream PokerHandProvider::ifs{PRECOM_FILE};

PokerHand PokerHandProvider::getPokerHand() {
    string s;
    ifs >> s;
    if (s.empty()) {
        ifs.clear();
        ifs.seekg(0, ios::beg);
    }
    ifs >> s;
    assert(s.size());
    return PokerHand(s);
}
