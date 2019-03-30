#include "EHSmontecarlo.h"

#include <bits/stdc++.h>

using namespace std;

void genr(int num, int id, int nodeid) {
    string fname = "out_" + to_string(id) + "_" + to_string(nodeid) + ".txt";
    ofstream f; f.open(fname);
    for (int i = 0; i < num; i++) {
        if (id == 0 && i % 1000 == 0) 
            printf("itteration: %d\n", i);
        HandStrength h;
        f << h.to_string() << '\n';
    }
    f.close();
}

int main() {
    int nthread, ngen, nodeid;
    cin >> nthread >> ngen >> nodeid;
    
    vector<thread> threads;
    // return 0;
    for (int tc = 0; tc < nthread; tc++) {
        threads.push_back(thread(genr, ngen, tc, nodeid));
    }
    
    for (int i = 0; i < nthread; ++i) {
        threads[i].join();
    }
    
    return 0;
}
