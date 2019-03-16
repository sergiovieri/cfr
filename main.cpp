#include "CFR.h"
#include "PokerGame/PokerState.h"

int main() {
    CFR<PokerState> cfr;
    cfr.train(1000000);
    return 0;
}