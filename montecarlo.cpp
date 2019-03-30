#include "omp/EquityCalculator.h"
#include "omp/CardRange.h"
#include "omp/Hand.h"
#include "omp/HandEvaluator.h"
#include "omp/Random.h"
#include <bits/stdc++.h>
using namespace std;

#define THREADS 1

omp::HandEvaluator he;

thread_local mt19937 randomGenerator = mt19937(random_device()());
thread_local uniform_int_distribution<> distribution(0, 51);
thread_local omp::XoroShiro128Plus rng = omp::XoroShiro128Plus(random_device()());
thread_local omp::FastUniformIntDistribution<unsigned, 8> fastDist(0, 51);

void fillCards(int num, long long &cards, long long &usedCards) {
  int rem = num - __builtin_popcountll(cards);
  // int i = 0;
  while (rem) {
    // int x = distribution(randomGenerator);
    int x = fastDist(rng);
    // int x = i++;
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

double getHS(long long ourCards, long long commCards, long long usedCards) {
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
  // for (int i = 0; i < 200; ++i) {
  //   long long newUsedCards = usedCards;
  //   long long oppCards = 0;

  //   fillCards(2, oppCards, newUsedCards);

  //   omp::Hand oppHand = getHand(commCards | oppCards);

  //   auto s2 = he.evaluate(oppHand);

  //   if (s1 > s2) wins += 2;
  //   else if (s1 == s2) wins++;
  //   // cout << "HS " << i << ": " << (double) wins / (i + 1) / 2 << '\n';
  // }
  return (double) wins / total / 2;
}

void montecarlo(long long ourCards, long long commCards, const int simulations, double *resultEHS, double *resultEHS2) {
  *resultEHS = 0;
  *resultEHS2 = 0;
  for (int i = 0; i < simulations; ++i) {
    long long usedCards = ourCards | commCards;
    long long newCommCards = commCards;
    fillCards(5, newCommCards, usedCards);
    double HS = getHS(ourCards, newCommCards, usedCards);
    *resultEHS += HS;
    *resultEHS2 += HS * HS;
  }
  *resultEHS /= simulations;
  *resultEHS2 /= simulations;
}

pair<double, double> getWinPercentage(long long holeCards, long long commCards, const int simulations) {
  double EHS = 0, EHS2 = 0;
  vector<thread> threads;
  array<double, THREADS> resultsEHS, resultsEHS2;
  for (int i = 0; i < THREADS; ++i) {
    threads.push_back(thread(montecarlo, holeCards, commCards, simulations / THREADS, &resultsEHS[i], &resultsEHS2[i]));
  }
  for (int i = 0; i < THREADS; ++i) {
    threads[i].join();
    EHS += resultsEHS[i];
    EHS2 += resultsEHS2[i];
  }
  return make_pair(EHS / THREADS, EHS2 / THREADS);
}

bool cmp(pair<double, double> a, pair<double, double> b) {
  // return a.first < b.first;
  return a.second < b.second;
}

void test_accuracy() {
  int simulate; cin >> simulate;
  double acc = 0.0;
  
  for (int it = 0; it < 1000; it++) {
    long long used = 0;
    long long hand = 0, comm = 0;
    fillCards(2, hand, used);
    fillCards(3, comm, used);
    
    auto truth = getWinPercentage(hand, comm, 10000);
    auto wp = getWinPercentage(hand, comm, simulate); 
    
    acc = max(acc, fabs(wp.second - truth.second));
    
    if (it % 20 == 0) {
      printf("acc: %lf, truth: %lf, own: %lf\n", acc, truth.second, wp.second);
    }
  }
  printf("End of accuracy test with %d simulations\n", simulate);
}

void interactive() {
  omp::EquityCalculator eq;
  eq.setTimeLimit(0.001);
  while (1) {
    string a, b;
    getline(cin, a); // our hand
    getline(cin, b); // community
    eq.start({a, "random"}, omp::CardRange::getCardMask(b));
    eq.wait();
    cout << eq.getResults().equity[0] << '\n';
    // pair<double, double> truth;
    // truth.first = ;
    // truth.second = truth.first * truth.first;
    // cout <<  truth.first << truth.second << '\n';

    long long holeCards = omp::CardRange::getCardMask(a);
    long long commCards = omp::CardRange::getCardMask(b);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
      getWinPercentage(holeCards, commCards, 1000);
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() * 1000.0 << "ms\n";
    auto wp = getWinPercentage(holeCards, commCards, 200);
    cout << wp.first << ", " << wp.second << '\n';
  }
}

int main() {
  while (1)
    test_accuracy();
  // interactive();

  // vector<pair<double, double>> v;

  // for (int i = 0; i < 1000; ++i) {
  //   long long holeCards = 0;
  //   long long usedCards = 0;
  //   fillCards(2, holeCards, usedCards);
  //   long long commCards = 0;
  //   fillCards(3, commCards, usedCards);
  //   v.push_back(getWinPercentage(holeCards, commCards, 1000));
  // }
  // sort(v.begin(), v.end(), cmp);
  // // for (auto i : v) {
  // //   cout << i.second << ", " << i.first << '\n';
  // // }
  // int b1 = 20, b2 = 2;
  // for (int i = 0; i < b1; ++i) {
  //   cout << v[v.size() * i / b1].second << ", " << v[v.size() * i / b1].first << '\n';
  // }
  // cout << v.back().second << ", " << v.back().first << '\n';

  // for (int i = 0; i < b1; ++i) {
  //   int l = v.size() * i / b1;
  //   int r = v.size() * (i + 1) / b1 - 1;
  //   cout << "Bucket " << i << "(" << v[l].second << " to " << v[r].second << ")\n";
  //   sort(v.begin() + l, v.begin() + r + 1);
  //   int sz = r - l + 1;
  //   for (int j = 0; j < b2; ++j) {
  //     cout << v[sz * j / b2 + l].first << '\n';
  //   }
  //   cout << v[r].first << '\n';
  // }
  return 0;
}

