#include "KuhnState.h"
#include <random>

using namespace std;

bool KuhnState::isTerminal() {
    unsigned long plays = history.length();
    if (plays > 1) {
        bool terminalPass = (history[plays - 1] == 'p');
        bool doubleBet = (history.substr(plays - 2) == "bb");
        if (terminalPass) return true;
        if (doubleBet) return true;
    }
    return false;
}

int KuhnState::getCurrentPlayer() {
    return static_cast<int>(history.length() % 2);
}

int KuhnState::getNumActions() {
    return 2;
}

KuhnState KuhnState::getNextState(int action) {
    return KuhnState(cards, history + (action ? "b" : "p"));
}

string KuhnState::getInfoSet() {
    return to_string(cards[getCurrentPlayer()]) + history;
}

pair<double, double> KuhnState::getTerminalValue() {
    unsigned long plays = history.length();
    int player = getCurrentPlayer();
    if (plays > 1) {
        bool terminalPass = (history[plays - 1] == 'p');
        bool doubleBet = (history.substr(plays - 2) == "bb");
        bool isPlayerCardHigher = cards[0] > cards[1];
        if (terminalPass) {
            if (history == "pp") {
                return isPlayerCardHigher ? make_pair(1.0, -1.0) : make_pair(-1.0, 1.0);
            } else {
                if (player == 0) return {1.0, -1.0};
                else return {-1.0, 1.0};
            }
        } else if (doubleBet) {
            if (isPlayerCardHigher) return {2.0, -2.0};
            else return {-2.0, 2.0};
        }
    }
    assert(0);
}

KuhnState::KuhnState() {
    random_device rd;
    shuffle(cards.begin(), cards.end(), mt19937(rd()));
}
