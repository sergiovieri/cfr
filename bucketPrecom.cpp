#include "EHSCalc/EHSCalc.h"

#include <bits/stdc++.h>

using namespace std;


void rec(int d, uint64_t val) {
    if (!d) {
        
        return;
    }
}
int main() {
    // A::f(100, 12);
    
    rec(5);
    for (int i = 0; i < 52;)
    for (int i = 0; i < 5; i++) {
        uint64_t holeCards = 0, oppCards = 0, commCards = 0, usedCards = 0;
        EHSCalc::fillCards(2, holeCards, usedCards);
        // EHSCalc::fillCards(0, commCards, usedCards);
        //
        double HS, HS2;
        tie(HS, HS2) = EHSCalc::getWinPercentageFull(holeCards, commCards);
        printf("HS, HS2: %lf %lf\n", HS, HS2);
    }
    
  return 0;
}