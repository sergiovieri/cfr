#include "CFR.h"
#include "BR.h"
#include "Parser.h"
#include "Arena.h"
#include "PokerGame/PokerState.h"

int main() {
//    CFR<PokerState> cfr;
//    cfr.train(10000000);
    auto strategy = Parser::parse("/Users/sergiovieri/cfrvtllp_20000000.txt");
    auto strategy2 = Parser::parse("/Users/sergiovieri/cfrvtllp_8000000.txt");
//    Arena<PokerState> arena(strategy, strategy2);
//    arena.start(1000000);
    BR<PokerState> br(strategy, 0);
    br.train(10000000);
    br = BR<PokerState>(strategy, 1);
    br.train(10000000);
    return 0;
}
