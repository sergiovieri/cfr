#include "omp/EquityCalculator.h"
#include "omp/CardRange.h"
#include "omp/Hand.h"
#include "omp/HandEvaluator.h"
#include "omp/Random.h"
#include <bits/stdc++.h>
using namespace std;

thread_local omp::HandEvaluator he;

thread_local mt19937 randomGenerator = mt19937(random_device()());
thread_local uniform_int_distribution<> distribution(0, 51);

void fillCards(int num, long long &cards, long long &usedCards) {
  int rem = num - __builtin_popcountll(cards);
  while (rem) {
    int x = distribution(randomGenerator);
    if ((1LL << x) & (usedCards)) continue;
    rem--;
    usedCards |= (1LL << x);
    cards |= (1LL << x);
  }
}

omp::Hand getHand(long long cards) {
  omp::Hand res = omp::Hand::empty();
  for (int i = 0; i < 52; ++i) {
    if ((1LL << i) & cards) {
      res += omp::Hand(i);
    }
  }
  return res;
}

double getHS(long long ourCards, long long commCards) {
  long long usedCards = ourCards | commCards;
  omp::Hand ourHand = getHand(commCards | ourCards);
  omp::Hand oppHandEmpty = getHand(commCards);
  auto s1 = he.evaluate(ourHand);
  int wins = 0, total = 0;
  for (int i = 0; i < 52; ++i) {
    if (usedCards & (1LU << i)) continue;
    for (int j = i + 1; j < 52; ++j) {
      if (usedCards & (1LU << j)) continue;
      omp::Hand oppHand = oppHandEmpty + omp::Hand(i) + omp::Hand(j);
      auto s2 = he.evaluate(oppHand);
      if (s1 > s2) wins += 2;
      else if (s1 == s2) wins++;
      ++total;
    }
  }
  return (double) wins / total / 2;
}

pair<double, double> getWinPercentageFlop(long long holeCards, long long commCards) {
  double EHS = 0, EHS2 = 0;
  int total = 0;
  long long used = holeCards | commCards;
  for (int i = 0; i < 52; ++i) {
    if (used & (1LU << i)) continue;
    for (int j = i + 1; j < 52; ++j) {
      if (used & (1LU << j)) continue;
      double HS = getHS(holeCards, commCards | (1LU << i) | (1LU << j));
      EHS += HS;
      EHS2 += HS * HS;
      total++;
    }
  }
  return make_pair(EHS / total, EHS2 / total);
}

pair<double, double> getWinPercentageTurn(long long holeCards, long long commCards) {
  double EHS = 0, EHS2 = 0;
  int total = 0;
  long long used = holeCards | commCards;
  for (int i = 0; i < 52; ++i) {
    if (used & (1LU << i)) continue;
    double HS = getHS(holeCards, commCards | (1LU << i));
    EHS += HS;
    EHS2 += HS * HS;
    total++;
  }
  return make_pair(EHS / total, EHS2 / total);
}

pair<double, double> getWinPercentageRiver(long long holeCards, long long commCards) {
  double HS = getHS(holeCards, commCards);
  return make_pair(HS, HS * HS);
}

pair<double, double> getWinPercentageFull(long long holeCards, long long commCards) {
  int num = __builtin_popcountll(commCards);
  if (num == 3) return getWinPercentageFlop(holeCards, commCards);
  else if (num == 4) return getWinPercentageTurn(holeCards, commCards);
  else if (num == 5) return getWinPercentageRiver(holeCards, commCards);
  assert(false);
}

struct HandStrength {
  static const int nROUND = 3;
  static const int nPLAYER = 2;
  static const int POWER = 2;

  // the whole state
  long long holeCards, oppCards;
  long long commCards[nROUND];

  enum round { FLOP, TURN, RIVER };
  enum handstr { HS, HS2 };


  double val[POWER][nROUND][nPLAYER];
  // example usage:
  // val[HS2][FLOP][0] => hand strength square, the round is flop, player 1
  // val[HS][FLOP][1]  => hand strength, the round is flop, player 2
  
  void test_acc() {
    double EPS = 1e-6;
    for (int r = 0; r < nROUND; r++) {
      for (int p = 0; p < nPLAYER; p++) {
        long long hand = p ? oppCards : holeCards;
        double v1, v2;
        tie(v1, v2) = getWinPercentageFull(hand, commCards[r]);
        assert(fabs(val[HS ][r][p] - v1) <= EPS);
        assert(fabs(val[HS2][r][p] - v2) <= EPS);
      }
    }
  }
  
  string to_string() {
    string ans = "";
    vector<int> hole, opp, comm;
    for (int i = 0; i < 52; i++) {
      long long ii = (1LL << i);
      if (ii & holeCards)
        hole.push_back(i);
      if (ii & oppCards)
        opp.push_back(i);
    }

    for (int j = 0; j < nROUND; j++) {
      long long cards = commCards[j] ^ (j ? commCards[j - 1] : 0);
      for (int i = 0; i < 52; i++) {
        if ((1LL << i) & cards)
          comm.push_back(i);
      }
    }

    for (int c : hole) {
      ans += '0' + c;
    }
    for (int c : opp) {
      ans += '0' + c;
    }
    for (int c : comm) {
      ans += '0' + c;
    }
    for (int i = 0; i < POWER; i++) {
      for (int j = 0; j < nROUND; j++) {
        for (int k = 0; k < nPLAYER; k++) {
          double v = val[i][j][k];
          if (v >= 1) ans += "1.3456";
          else ans += std::to_string(v).substr(2, 6);
        }
      }
    }
    return ans;
  }

  // generate random state, generate HS and HS^2 for flop, turn and river.
  HandStrength() {
    holeCards = oppCards = commCards[FLOP] = 0;
    long long usedCards = 0;
    fillCards(2, holeCards, usedCards);
    fillCards(2,  oppCards, usedCards);
    fillCards(2, commCards[FLOP], usedCards);

    for (int r = 0; r < nROUND; r++) {
      if (r)
        commCards[r] = commCards[r - 1];
      fillCards(3 + r, commCards[r], usedCards);
      for (int p = 0; p < nPLAYER; p++) {
        long long hand = p ? oppCards : holeCards;
        tie(val[HS][r][p], val[HS2][r][p]) = getWinPercentageFull(hand, commCards[r]);
      }
    }
  }

  HandStrength(string s) {
    stringstream ss(s);
    holeCards = oppCards = 0;
    for (int i = 0; i < 2; i++)
      holeCards ^= (1LL << getTwoDigit(ss));

    for (int i = 0; i < 2; i++)
      oppCards  ^= (1LL << getTwoDigit(ss));

    for (int r = 0; r < nROUND; r++) {
      int ncard = r ? 1 : 3;

      commCards[r] = (r ? commCards[r - 1] : 0);
      for (int i = 0; i < ncard; i++)
        commCards[r] ^= (1LL << getTwoDigit(ss));
    }

    for (int i = 0; i < POWER; i++)
      for (int j = 0; j < nROUND; j++)
        for (int k = 0; k < nPLAYER; k++)
          val[i][j][k] = getDoubleSixDigit(ss);

  }

 private:
  int getTwoDigit(stringstream &ss) {
    char s;
    ss >> s;
    return s - '0';
  }

  double getDoubleSixDigit(stringstream &ss) {
    char s[6];
    for (int i = 0; i < 6; i++) { ss >> s[i]; }
    if (s[1] == '.') return 1.0;
    double ans = 0.0;
    for (int i = 5; i >= 0; i--) 
      ans = (ans + s[i] - '0') / 10;
    return ans;
  }

  string to_string(int card) {
    return "" + '0' + card;
  }
};

