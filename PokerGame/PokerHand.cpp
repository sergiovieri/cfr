#include "PokerHand.h"
#include "EHSCalc.h"

PokerHand::PokerHand() {
    holeCards = oppCards = commCards[FLOP] = 0;
    uint64_t usedCards = 0;
    EHSCalc::fillCards(2, holeCards, usedCards);
    EHSCalc::fillCards(2, oppCards, usedCards);
    EHSCalc::fillCards(2, commCards[FLOP], usedCards);

    for (int r = 0; r < nROUND; r++) {
        if (r) commCards[r] = commCards[r - 1];
        EHSCalc::fillCards(3 + r, commCards[r], usedCards);
        for (int p = 0; p < nPLAYER; p++) {
            uint64_t hand = p ? oppCards : holeCards;
            tie(val[HS][r][p], val[HS2][r][p]) = EHSCalc::getWinPercentageFull(hand, commCards[r]);
        }
    }

    winner = EHSCalc::getWinner(holeCards, oppCards, commCards[RIVER]);
}

PokerHand::PokerHand(const string &s) {
    stringstream ss(s);
    holeCards = oppCards = 0;
    for (int i = 0; i < 2; i++)
        holeCards ^= (1LU << getTwoDigit(ss));

    for (int i = 0; i < 2; i++)
        oppCards ^= (1LU << getTwoDigit(ss));

    for (int r = 0; r < nROUND; r++) {
        int ncard = r ? 1 : 3;

        commCards[r] = (r ? commCards[r - 1] : 0);
        for (int i = 0; i < ncard; i++)
            commCards[r] ^= (1LU << getTwoDigit(ss));
    }

    for (auto &i : val) {
        for (auto &j : i) {
            for (double &k : j) {
                k = getDoubleSixDigit(ss);
            }
        }
    }
    winner = EHSCalc::getWinner(holeCards, oppCards, commCards[RIVER]);
//    test_acc();
}

string PokerHand::to_string() {
    string ans;
    vector<int> hole, opp, comm;
    for (uint8_t i = 0; i < 52; i++) {
        uint64_t ii = (1LU << i);
        if (ii & holeCards)
            hole.push_back(i);
        if (ii & oppCards)
            opp.push_back(i);
    }

    for (int j = 0; j < nROUND; j++) {
        uint64_t cards = commCards[j] ^(j ? commCards[j - 1] : 0);
        for (uint8_t i = 0; i < 52; i++) {
            if ((1LU << i) & cards)
                comm.push_back(i);
        }
    }

    for (int c : hole) {
        ans += static_cast<char>('0' + c);
    }
    for (int c : opp) {
        ans += static_cast<char>('0' + c);
    }
    for (int c : comm) {
        ans += static_cast<char>('0' + c);
    }
    for (auto &i : val) {
        for (auto &j : i) {
            for (double v : j) {
                if (v >= 1) ans += "1.3456";
                else ans += std::to_string(v).substr(2, 6);
            }
        }
    }
    return ans;
}

void PokerHand::swapPlayers() {
    swap(holeCards, oppCards);
    for (int r = 0; r < nROUND; ++r) {
        swap(val[HS][r][0], val[HS][r][1]);
        swap(val[HS2][r][0], val[HS2][r][1]);
    }
    winner *= -1;
}

string PokerHand::to_string(uint8_t card) {
    return string(1, '0' + card);
}

double PokerHand::getDoubleSixDigit(stringstream &ss) {
    char s[6];
    for (char &i : s) ss >> i;
    if (s[1] == '.') return 1.0;
    double ans = 0.0;
    for (int i = 5; i >= 0; i--)
        ans = (ans + s[i] - '0') / 10;
    return ans;
}

int PokerHand::getTwoDigit(stringstream &ss) {
    char s;
    ss >> s;
    return s - '0';
}

void PokerHand::test_acc() {
    double EPS = 1e-5;
    for (int r = 0; r < nROUND; r++) {
        for (int p = 0; p < nPLAYER; p++) {
            long long hand = p ? oppCards : holeCards;
            double v1, v2;
            tie(v1, v2) = EHSCalc::getWinPercentageFull(hand, commCards[r]);
            assert(fabs(val[HS][r][p] - v1) <= EPS);
            assert(fabs(val[HS2][r][p] - v2) <= EPS);
        }
    }
}
