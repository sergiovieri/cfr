#ifndef CFR_STATE_H
#define CFR_STATE_H

#include <vector>
#include <string>

using namespace std;

class State {
public:
    virtual bool isTerminal() = 0;

    virtual int getCurrentPlayer() = 0;

    virtual int getNumActions() = 0;

    virtual State *getNextState(int action) = 0;

    virtual string getInfoSet() = 0;

    virtual pair<double, double> getTerminalValue() = 0;

    virtual ~State() = default;
};


#endif //CFR_STATE_H
