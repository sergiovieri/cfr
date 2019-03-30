#include "EHSCalc/EHSCalc.h"
#include <bits/stdc++.h>

#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) x.begin(), x.end()
#define sz(x) (int)(x).size()
typedef long long ll;
typedef pair<int, int> pii;
typedef vector<int> vi;
#define ft first
#define sc second
#define pb push_back
#define vi vector<int>

using namespace std;

char encode(int a) {
    return '0' + a;   
}

const array<pair<double, double>, 20> flopBucket = {
        {
                {0.101456, 0.188832},
                {0.123186, 0.231488},
                {0.141547, 0.267828},
                {0.160202, 0.303546},
                {0.178681, 0.337738},
                {0.199609, 0.365623},
                {0.219483, 0.393674},
                {0.239416, 0.42229},
                {0.261036, 0.450845},
                {0.286615, 0.475709},
                {0.316226, 0.503452},
                {0.345526, 0.536103},
                {0.373256, 0.564127},
                {0.410473, 0.594363},
                {0.454793, 0.631974},
                {0.505057, 0.672165},
                {0.567317, 0.714859},
                {0.647643, 0.767578},
                {0.740123, 0.825548},
                {1, 0.911215}
        }};
        
char getBucket(double HS, double HS2) {
    int bucket1, bucket2;
    for (int i = 19; i >= 0; i--) {
        if (HS2 <= flopBucket[i].first) { 
            bucket1 = i;
        }
    }
    
    bucket2 = (HS <= flopBucket[bucket1].second ? 0 : 1);
    
    return encode(bucket1 * 2 + bucket2);
}

pii intToCard(int x) {
    int rank = x / 4;
    int suit = x % 4;
    return make_pair(rank, suit);
}

int cardToInt(pii c) {
    return c.first * 4 + c.second;
}

bool cmpRank(pii a, pii b) {
    return a.first < b.first;
}

array<pii, 5> compute(int a, int b, int c, int d, int e) {
    array<pii, 5> res = {intToCard(a), intToCard(b), intToCard(c), intToCard(d), intToCard(e)};
    sort(&res[0], &res[2], cmpRank);
    sort(&res[2], &res[5], cmpRank);
    int mapping[] = {-1, -1, -1, -1}, tot = 0;
    rep(i, 0, 5) {
        auto &card = res[i];
        int csuit = card.second;
        if (mapping[csuit] == -1) {
            mapping[csuit] = tot++;
        }
        card.second = mapping[csuit];
    }
    return res;
}

int main() {
    ofstream f; f.open("flopBucket.txt");
    f << "{\n";
    
    set<array<pii, 5>> s;
    
    int it = 0;
    rep(i, 0, 52) rep(j, i + 1, 52) {
        rep(k, 0, 52) {
        if (k == i || k == j) continue;
        rep(l, k + 1, 52) {
            if (l == i || l == j) continue;
            rep(m, l + 1, 52) {
                if (m == i || m == j) continue;
                
                auto cur = compute(i, j, k, l, m);
                if (s.count(cur)) continue;
                s.insert(cur);
                
                int cards[5];
                rep(x, 0, 5) cards[x] = cardToInt(cur[x]);
                
                uint64_t hole = 0, comm = 0;
                rep(x, 0, 2) hole |= (1LU << cards[x]);
                rep(x, 2, 5) comm |= (1LU << cards[x]);
                
                double HS, HS2;
                tie(HS, HS2) = EHSCalc::getWinPercentageFull(hole, comm);
                
                string key = "";
                for (int i = 0; i < 5; i++) {
                    key += encode(cards[i]);
                }
                
                string val = "\"" + getBucket(HS, HS2);
                val += "\"";
                f << "\"" << key << "\":" << val << ",\n";
                
                it++;
                if (it == 5000) exit(0);
            }
        }
      }
    }
    
    f << "}\n";
    
  return 0;
}