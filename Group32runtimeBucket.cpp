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

const array<pair<double, double>, 20> turnBucket = {
        {
                {0.0526015, 0.123803},
                {0.0749334, 0.186605},
                {0.0997095, 0.234695},
                {0.120278, 0.270037},
                {0.141065, 0.304293},
                {0.165198, 0.336737},
                {0.189298, 0.375176},
                {0.213662, 0.410738},
                {0.240839, 0.433663},
                {0.272517, 0.471454},
                {0.299626, 0.508169},
                {0.335838, 0.537275},
                {0.381585, 0.575516},
                {0.430142, 0.620026},
                {0.495838, 0.665964},
                {0.563351, 0.720575},
                {0.641879, 0.768884},
                {0.720254, 0.823518},
                {0.828802, 0.87186},
                {1, 0.947552}
        }
};


int getTurnInfo(double ehs, double ehs2) {
    for (int bucket = 0; bucket < 20; ++bucket) {
        if (ehs2 <= turnBucket[bucket].first) {
            if (ehs <= turnBucket[bucket].second) {
                return bucket * 2 + 0;
            } else {
                return bucket * 2 + 1;
            }
        }
    }
    assert(false);
    __builtin_unreachable();
}

int getRiverInfo(double ehs, double ehs2) {
    return (int) floor((long double) ehs * 40.0);
}

int main() {
    ll hand, comm;
    while (scanf("%lld %lld", &hand, &comm) != EOF) { // only works for turn and river
        
        double HS, HS2;
        tie(HS, HS2) = EHSCalc::getWinPercentageFull(hand, comm);
        
        int count = __builtin_popcount(comm), bucketId = -1;
        if (count == 4)
            bucketId = getTurnInfo(HS, HS2);
        else if (count == 5)
            bucketId = getRiverInfo(HS, HS2);
        
        printf("%d\n", bucketId);
	fflush(stdout);
    }
    
    return 0;
}
