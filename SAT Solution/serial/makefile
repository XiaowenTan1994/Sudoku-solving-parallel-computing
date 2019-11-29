serial:
	g++ -std=c++11 sudoku2sat.cpp -o sudoku2sat
	g++ -std=c++11 sat2sudoku.cpp -o sat2sudoku
	g++ -std=c++11 -c Cdcl.cpp
	g++ -std=c++11 -c CdclDependencyGraph.cpp
	g++ -std=c++11 -c Clause.cpp
	g++ -std=c++11 -c Cnf.cpp
	g++ -std=c++11 -c CnfParser.cpp
	g++ -std=c++11 -c Variable.cpp
	g++ -std=c++11 serialSolver.cpp -o serial Cdcl.o CdclDependencyGraph.o Clause.o Cnf.o CnfParser.o Variable.o
