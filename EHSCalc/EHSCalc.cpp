#include "EHSCalc.h"

mt19937 EHSCalc::randomGenerator{random_device()()};
uniform_int_distribution<> EHSCalc::distribution{0, 51};
omp::HandEvaluator EHSCalc::he{};

void EHSCalc::fillCards(uint8_t num, uint64_t &cards, uint64_t &usedCards) {
    uint8_t rem = num - __builtin_popcountll(cards);
    while (rem) {
        uint8_t x = distribution(randomGenerator);
        if ((1LU << x) & (usedCards)) continue;
        rem--;
        usedCards |= (1LU << x);
        cards |= (1LU << x);
    }
}

omp::Hand EHSCalc::getHand(uint64_t cards) {
    omp::Hand res = omp::Hand::empty();
    for (uint8_t i = 0; i < 52; ++i) {
        if ((1LU << i) & cards) {
            res += omp::Hand(i);
        }
    }
    return res;
}

double EHSCalc::getHS(uint64_t ourCards, uint64_t commCards) {
    uint64_t usedCards = ourCards | commCards;
    omp::Hand ourHand = getHand(commCards | ourCards);
    omp::Hand oppHandEmpty = getHand(commCards);
    
    auto s1 = he.evaluate(ourHand);

    omp::Hand tempHand = getHand((1 << 7) - 1);
    cout << tempHand.count() << " " << he.evaluate(tempHand) << '\n';

    int wins = 0, total = 0;
    for (uint8_t i = 0; i < 52; ++i) {
        if (usedCards & (1LU << i)) continue;
        for (uint8_t j = i + 1; j < 52; ++j) {
            if (usedCards & (1LU << j)) continue;
            omp::Hand oppHand = oppHandEmpty + omp::Hand(i) + omp::Hand(j);
            auto s2 = he.evaluate(oppHand);
            if (s1 > s2) wins += 2;
            else if (s1 == s2) wins++;
            ++total;
        }
    }
    cout << "HS " << ourCards << " " << commCards << ": " << (double) wins / total / 2 << '\n';
    return (double) wins / total / 2;
}

pair<double, double> EHSCalc::getWinPercentageFlop(uint64_t holeCards, uint64_t commCards) {
    double EHS = 0, EHS2 = 0;
    int total = 0;
    uint64_t used = holeCards | commCards;
    for (uint8_t i = 0; i < 52; ++i) {
        if (used & (1LU << i)) continue;
        for (uint8_t j = i + 1; j < 52; ++j) {
            if (used & (1LU << j)) continue;
            double HS = getHS(holeCards, commCards | (1LU << i) | (1LU << j));
            EHS += HS;
            EHS2 += HS * HS;
            total++;
        }
    }
    return make_pair(EHS / total, EHS2 / total);
}

pair<double, double> EHSCalc::getWinPercentageTurn(uint64_t holeCards, uint64_t commCards) {
    double EHS = 0, EHS2 = 0;
    int total = 0;
    uint64_t used = holeCards | commCards;
    for (uint8_t i = 0; i < 52; ++i) {
        if (used & (1LU << i)) continue;
        double HS = getHS(holeCards, commCards | (1LU << i));
        EHS += HS;
        EHS2 += HS * HS;
        total++;
    }
    return make_pair(EHS / total, EHS2 / total);
}

pair<double, double> EHSCalc::getWinPercentageRiver(uint64_t holeCards, uint64_t commCards) {
    double HS = getHS(holeCards, commCards);
    return make_pair(HS, HS * HS);
}

pair<double, double> EHSCalc::getWinPercentageFull(uint64_t holeCards, uint64_t commCards) {
    int num = __builtin_popcountll(commCards);
    if (num == 3) return getWinPercentageFlop(holeCards, commCards);
    else if (num == 4) return getWinPercentageTurn(holeCards, commCards);
    else if (num == 5) return getWinPercentageRiver(holeCards, commCards);
    __builtin_unreachable();
}

int EHSCalc::getWinner(uint64_t holeCards, uint64_t oppCards, uint64_t commCards) {
    auto ourHand = getHand(holeCards | commCards);
    auto oppHand = getHand(oppCards | commCards);
    auto s1 = he.evaluate(ourHand);
    auto s2 = he.evaluate(oppHand);

    if (s1 < s2) return -1;
    else if (s1 > s2) return 1;
    else return 0;
}
