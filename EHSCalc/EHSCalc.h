#ifndef CFR_EHSCALC_H
#define CFR_EHSCALC_H

#include <iostream>
#include <random>
#include "../omp/HandEvaluator.h"

using namespace std;

class EHSCalc {
public:
    static pair<double, double> getWinPercentageFull(uint64_t holeCards, uint64_t commCards);

    static int getWinner(uint64_t holeCards, uint64_t oppCards, uint64_t commCards);

    static void fillCards(uint8_t num, uint64_t &cards, uint64_t &usedCards);

private:
    static omp::HandEvaluator he;   
    
    static mt19937 randomGenerator;

    static uniform_int_distribution<> distribution;

    static omp::Hand getHand(uint64_t cards);

    static double getHS(uint64_t ourCards, uint64_t commCards);

    static pair<double, double> getWinPercentageFlop(uint64_t holeCards, uint64_t commCards);

    static pair<double, double> getWinPercentageTurn(uint64_t holeCards, uint64_t commCards);

    static pair<double, double> getWinPercentageRiver(uint64_t holeCards, uint64_t commCards);
};

#endif //CFR_EHSCALC_H
