parallel:
	g++ -std=c++11 sudoku2sat.cpp -o sudoku2sat
	g++ -std=c++11 sat2sudoku.cpp -o sat2sudoku
	mpic++ -std=c++11 -c Cdcl.cpp
	mpic++ -std=c++11 -c CdclDependencyGraph.cpp
	mpic++ -std=c++11 -c Clause.cpp
	mpic++ -std=c++11 -c Cnf.cpp
	mpic++ -std=c++11 -c CnfParser.cpp
	mpic++ -std=c++11 -c Variable.cpp
	mpic++ -std=c++11 -c Model.cpp
	mpic++ -std=c++11 -c Master.cpp
	mpic++ -std=c++11 -c SlaveWorker.cpp
	mpic++ -std=c++11 parallelSAT.cpp -o parallel Cdcl.o CdclDependencyGraph.o Clause.o Cnf.o CnfParser.o Model.o Variable.o Master.o SlaveWorker.o
