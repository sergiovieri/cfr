#include "omp/EquityCalculator.h"
#include "omp/CardRange.h"
#include "omp/Hand.h"
#include "omp/HandEvaluator.h"
#include <bits/stdc++.h>
using namespace std;

omp::HandEvaluator he;

void fillCards(int num, long long &cards, long long &usedCards) {
  while (__builtin_popcountll(cards) < num) {
    int x = rand() % 52;
    if ((1LL << x) & (usedCards)) continue;
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

int montecarlo(long long ourCards, long long commCards, long long usedCards) {
  fillCards(5, commCards, usedCards);
  long long oppCards = 0;
  fillCards(2, oppCards, usedCards);
  assert(__builtin_popcountll(usedCards) == 9);
  omp::Hand ourHand = getHand(commCards | ourCards);
  omp::Hand oppHand = getHand(commCards | oppCards);
  // cout << "start " << commCards << " " << ourCards << " " << oppCards << endl;
  auto s1 = he.evaluate(ourHand);
  // cout << "done 1" << endl;
  auto s2 = he.evaluate(oppHand);
  // cout << "finished " << s1 << " " << s2 << endl;
  if (s1 > s2) return 2;
  else if (s1 == s2) return 1;
  else return 0;
}

double getWinPercentage(string hole, long long commCards, int simulations) {
  int wins = 0;
  long long holeMask = omp::CardRange::getCardMask(hole);
  for (int i = 0; i < simulations; ++i) {
    wins += montecarlo(holeMask, commCards, commCards | holeMask);
  }
  return (double)wins / simulations / 2.0;
}

int main() {
  omp::EquityCalculator eq;
  eq.setTimeLimit(0.001);
  while (1) {
    string a;
    getline(cin, a);
    omp::Hand hand = getHand(omp::CardRange::getCardMask(a));
    cout << he.evaluate(hand) << endl;
  }
  return 0;
}

