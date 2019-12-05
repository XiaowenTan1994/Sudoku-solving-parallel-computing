# Sudoku-solving-parallel-computing
## Pencil and paper solution
We test this method on USC HPC server. Instructions to run the program:
1.put all files in paper and pencil solution holder in same holder and use "sudoku.txt" as input.
2.type command "g++ -std=c++11 -o main main.cpp -lpthread"
3.run code "./main"

## Convert Sudoku to SAT
We test this method on USC HPC server. To solve Sudoku, the first thing to do is to convert Sudoku to CNF formula. The second is to solve this SAT problem. Finally, convert output CNF file to Sudoku.
Name the input Sudoku file as input.txt
Example of running serial code
```bash
g++ -std=c++11 -o sudoku2sat sudoku2sat.cpp
g++ -std=c++11 -o sat2sudoku sat2sudoku.cpp
make serial
./sudoku2sat
./serial input.cnf
./sat2sudoku
```
Example of running parallel code in 4 cores
```bash
source /usr/usc/openmpi/1.8.8/setup.sh
g++ -std=c++11 -o sudoku2sat sudoku2sat.cpp
g++ -std=c++11 -o sat2sudoku sat2sudoku.cpp
make parallel
./sudoku2sat
salloc --ntasks=64
source /usr/usc/openmpi/1.8.8/setup.sh
mpirun -np 4 ./parallel input.cnf
logout
./sat2sudoku
```
