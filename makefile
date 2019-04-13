CXX = G++
CXXFLAGS = -std=c++11 -Ofast -pthread
omp = omp/CardRange.o omp/CombinedRange.o omp/EquityCalculator.o omp/HandEvaluator.o

$(omp): %.o: %.cpp 
	$(CXX) -c $< -o $@ $(CXXFLAGS)
	
EHSmontecarlo.o: EHSmontecarlo.h
	$(CXX) -c EHSmontecarlo.h -o EHSmontecarlo.o $(CXXFLAGS)

EHSCalc/EHSCalc.o: %.o: %.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

montecarlo: montecarlo.cpp $(omp)
	$(CXX) montecarlo.cpp $(omp) -o montecarlo $(CXXFLAGS)
		
verify: verify.cpp EHSmontecarlo.o
	$(CXX) verify.cpp EHSmontecarlo.o -o verify $(CXXFLAGS)
	
generate: generate.cpp EHSmontecarlo.o
	$(CXX) generate.cpp EHSmontecarlo.o -o generate $(CXXFLAGS)
	
coba: coba.cpp EHSCalc/EHSCalc.o $(omp)
	$(CXX) coba.cpp EHSCalc/EHSCalc.o $(omp) -o coba $(CXXFLAGS)

runtimeBucket: runtimeBucket.cpp EHSCalc/EHSCalc.o $(omp)
	$(CXX) runtimeBucket.cpp EHSCalc/EHSCalc.o $(omp) -o runtimeBucket $(CXXFLAGS)

bucketPrecom: bucketPrecom.cpp EHSCalc/EHSCalc.o $(omp)
	$(CXX) bucketPrecom.cpp EHSCalc/EHSCalc.o $(omp) -o bucketPrecom $(CXXFLAGS)

clean: 
	rm EHSCalc/*.o omp/*.o *.o verify generate coba montecarlo computeBucket