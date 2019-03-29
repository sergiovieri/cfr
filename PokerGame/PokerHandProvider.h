#ifndef CFR_POKERHANDPROVIDER_H
#define CFR_POKERHANDPROVIDER_H

#define PRECOM_FILE  "hs-precom.txt"

#include <fstream>
#include "PokerHand.h"

using namespace std;

class PokerHandProvider {
public:
    static PokerHand getPokerHand();

private:
    static ifstream ifs;

};


#endif //CFR_POKERHANDPROVIDER_H
