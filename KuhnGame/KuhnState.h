#include <utility>

#include <utility>

#ifndef CFR_KUHNSTATE_H
#define CFR_KUHNSTATE_H


#include "../Game/State.h"
#include <string>

class KuhnState {
public:
    bool isTerminal();

    int getCurrentPlayer();

    int getNumActions();

    KuhnState getNextState(int i);

    string getInfoSet();

    pair<double, double> getTerminalValue();

    KuhnState();

    KuhnState(vector<int> cards, string history) : cards(move(cards)), history(move(history)) {}

    ~KuhnState() = default;

private:

    vector<int> cards = {1, 2, 3};
    string history = "";
};


#endif //CFR_KUHNSTATE_H
