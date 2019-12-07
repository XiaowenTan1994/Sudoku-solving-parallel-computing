# Sudoku-solving-parallel-computing
## Simple Backtracking
Both cpu_backtracking.cpp and gpu_backtracking.cu was tested on USC HPC Server. 

Input file prepare:
```bash
Insert your Sudoku probelm in a .txt file.
Use '0' represents a empty space.
Use space to separate digits.
Each row goes into a new line.
```
Run serial code:
```bash
Compile: 
	"g++ -std=c++11 -o cpurun cpu_backtracking.cpp"
run: 
	./cpurun ...[filepath]/[fullname of you Sudoku file] 
```
Running parallcl code:
```bash
Method 1：
	Write your own .sl file:
		#!/bin/sh
		#SBATCH --gres=gpu:1

		#SBATCH -o cuda1.out

		#SBATCH -e cuda1.error

		source /usr/usc/cuda/default/setup.sh

		nvcc -o cudasudoku gpu_backtracking.cu

		./cudasudoku [threadperblock] [block number] [full path of your Sudoku file]
	Submit to server by running "sbatch [filename].sl"

Method 2:
	Require GPU resources on Server:
		salloc --gres=gpu:k20:1 --time=02:00:00
		source /usr/usc/cuda/default/setup.sh
	Compile .cu file:
		nvcc -o cudasudoku gpu_backtracking.cu
	Run .cu file:
		./cudasudoku [threadperblock] [block number] [full path of your Sudoku file]
```


## Pencil and paper solution
We test this method on USC HPC server. Instructions to run the program:
```bash
1.put all files in paper and pencil solution holder in same holder and use "sudoku.txt" as input.
2.type command "g++ -std=c++11 -o main main.cpp -lpthread"
3.run code "./main"
```

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
