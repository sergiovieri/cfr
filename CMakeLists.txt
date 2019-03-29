cmake_minimum_required(VERSION 3.13)
project(cfr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")

add_executable(cfr main.cpp Game/State.cpp Game/State.h CFR.h KuhnGame/KuhnState.cpp KuhnGame/KuhnState.h Node.cpp Node.h PokerGame/PokerState.h PokerGame/PokerState.cpp omp/CardRange.cpp omp/CardRange.h omp/CombinedRange.cpp omp/CombinedRange.h omp/Constants.h omp/EquityCalculator.h omp/EquityCalculator.cpp omp/Hand.h omp/HandEvaluator.h omp/HandEvaluator.cpp omp/Random.h omp/Util.h BR.h Parser.cpp Parser.h Arena.h Abstractors/AbstractorPreflop.h Abstractors/AbstractorFlop.h Abstractors/AbstractorWP.h PokerGame/EHSCalc.cpp PokerGame/EHSCalc.h PokerGame/PokerHand.cpp PokerGame/PokerHand.h)