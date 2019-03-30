#include "EHSmontecarlo.h"

#include <bits/stdc++.h>

using namespace std;


int main() {
    string s;
    int tc = 0;
    // return 0;
    while (cin >> s) {
        tc++;
        if (tc % 100 == 0) 
            cout << tc << endl;
            
        HandStrength test(s);
        
        assert(test.to_string() == s);
        
        test.test_acc();
    }
    return 0;
}