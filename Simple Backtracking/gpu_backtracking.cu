#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstring>
#include <time.h>

#include <cuda_runtime.h>
#include <algorithm>
#include <curand.h>


#define N 9
#define n 3
#define size 9

__device__ void printtable(int* table)
{
	printf("-------------------------------\n");
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			printf("%d ", table[i * size + j]);
		}
		printf("\n");
	}

	printf("-------------------------------\n");
}

__device__ bool checkEntire(const int* table)
{
	int occur[9];
	for (int i = 0; i < 9; i++) occur[i] = 0;

	//check row
	for (int i = 0; i < 9; i++)
	{
		for (int h = 0; h < 9; h++) occur[h] = 0;
		for (int j = 0; j < 9; j++)
		{
			int val = table[i * 9 + j];
			if (val != 0)
			{
				if (occur[val - 1] == 1)
				{
					return false;
				}
				else
				{
					occur[val - 1] = 1;
				}
			}
		}
	}

	//check column
	for (int i = 0; i < 9; i++)
	{
		for (int h = 0; h < 9; h++) occur[h] = 0;
		for (int j = 0; j < 9; j++)
		{
			int val = table[j * 9 + i];
			if (val != 0)
			{
				if (occur[val - 1] == 1)
				{
					return false;
				}
				else
				{
					occur[val - 1] = 1;
				}
			}
		}
	}

	//check box
	for (int a = 0; a < 3; a++)
	{
		for (int b = 0; b < 3; b++)
		{
			for (int h = 0; h < 9; h++) occur[h] = 0;

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int val = table[(a * 3 + i) * 9 + (b * 3 + j)];
					if (val != 0)
					{
						if (occur[val - 1] == 1)
						{
							return false;
						}
						else
						{
							occur[val - 1] = 1;
						}
					}
				}
			}
		}
	}

	return true;

}

__device__ bool validBoard(const int* board, int changed) {

	int r = changed / 9;
	int c = changed % 9;

	// if changed is less than 0, then just default case
	if (changed < 0) {
		return checkEntire(board);
	}

	if ((board[changed] < 1) || (board[changed] > 9)) {
		return false;
	}

	bool seen[N];
	for (int h = 0; h < 9; h++) seen[h] = false;


	// check if row is valid
	for (int i = 0; i < N; i++) {
		int val = board[r * N + i];

		if (val != 0) {
			if (seen[val - 1]) {
				return false;
			}
			else {
				seen[val - 1] = true;
			}
		}
	}

	// check if column is valid
	for (int h = 0; h < 9; h++) seen[h] = false;
	for (int j = 0; j < N; j++) {
		int val = board[j * N + c];

		if (val != 0) {
			if (seen[val - 1]) {
				return false;
			}
			else {
				seen[val - 1] = true;
			}
		}
	}

	// finally check if the sub-board is valid
	int ridx = r / n;
	int cidx = c / n;

	for (int h = 0; h < 9; h++) seen[h] = false;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int val = board[(ridx * n + i) * N + (cidx * n + j)];

			if (val != 0) {
				if (seen[val - 1]) {
					return false;
				}
				else {
					seen[val - 1] = true;
				}
			}
		}
	}

	// if we get here, then the board is valid
	return true;
}

__device__ bool checkValid(int* table, int row, int col, int val)
{
	//check along row, column and cube for the validation of this assignment
	//the start index of coresponding cube 
	for (int h = 0; h < 9; h++)
	{
		//check along row
		if (table[row * size + h] == val)
		{
			return false;
		}
	}

	for (int h = 0; h < 9; h++)
	{
		//check along column
		if (table[h * size + col] == val)
		{
			return false;
		}
	}

	//check within a cube
	int i = row - row % 3;
	int j = col - col % 3;
	for (int h = 0; h < 3; h++)
	{
		for (int k = 0; k < 3; k++)
		{
			int idx_r = i + h;
			int idx_c = j + k;
			if (table[idx_r * size + idx_c] == val) return false;
		}
	}

	return true;
}

bool validChecking(int* table, int row, int col, int val)
{
	//check along row, column and cube for the validation of this assignment
	//the start index of coresponding cube 
	for (int h = 0; h < 9; h++)
	{
		//check along row
		if (table[row * size + h] == val)
		{
			return false;
		}
	}

	for (int h = 0; h < 9; h++)
	{
		//check along column
		if (table[h * size + col] == val)
		{
			return false;
		}
	}

	//check within a cube
	int i = row - row % 3;
	int j = col - col % 3;
	for (int h = 0; h < 3; h++)
	{
		for (int k = 0; k < 3; k++)
		{
			int idx_r = i + h;
			int idx_c = j + k;
			if (table[idx_r * size + idx_c] == val) return false;
		}
	}

	return true;
}

int initial_search(int* new_table, int* old_table)
{
	//generating more possible tables for BFS using
	//find a empty spot in the initial table
	bool found = false;
	int i = 0;
	int count = 0;
	while (i < size * size && !found)
	{
		
		if (old_table[i] == 0)
		{
			found = true;
			//find a valid value for the board
			for (int num = 1; num <= 9; num++)
			{
				int r = i / size;
				int c = i % size;
				if (validChecking(old_table, r, c, num))
				{
					count++;
					//copy to new_board
					for (int j = 0; j < size * size; j++)
					{
						new_table[count * size * size + j] = old_table[j];
					}
					new_table[count * size * size + i] = num;
				}
			}
		}
	}
	return count;
}


void loadToFile(const char* outFile, int* table)
{
	FILE* out = fopen(outFile, "w");
	if (out == NULL)
	{
		printf("Could not open file\n!");
		return;
	}


	fprintf(out, "---------------------------------\n");
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			fprintf(out, "%d ", table[i * size + j]);
		}
		fprintf(out, "\n");
	}
	printf("Solution has loaded to file\n");

}



__global__ void cudaBFS(int* old_table, int* new_table, int total_tables, int* table_index, int* empty_space, int* empty_space_count)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	while (index < total_tables)
	{
		int cur_position = index * size * size;
		int* cur_table = old_table + cur_position;

		//find the next empty spot
		bool found = false;
		int i = 0;

		//find an empty spot
		while ((i < size * size) && (!found))
		{
			if (cur_table[i] == 0)
			{
				found = true;
				//row, column of this position
				int row = i / size;
				int col = i % size;

				//find out the numbers that work at this position
				for (int num = 1; num <= 9; num++)
				{
					if (checkValid(cur_table, row, col, num))
					{
						//copy the board with the empty box filled with num
						//int newindex = *table_index;
						//atomicAdd(table_index, 1);
						int newindex = atomicAdd(table_index, 1);

						//count the number of empty spaces in the new_table
						int count = 0;
						//global memory---should not modify!!!!
						//cur_table[i] = num;
						for (int h = 0; h < size; h++)
						{
							for (int k = 0; k < size; k++)
							{
								new_table[newindex * size * size + h * size + k] = cur_table[h * size + k];
								//record the position of the empty space and the total empty spaces in this table
								if (cur_table[h * size + k] == 0)
								{
									if (h != row || k != col)
									{
										empty_space[newindex * size * size + count] = h * size + k;
										count++;
									}
									else if(h==row && k==col)
									{
										new_table[newindex * size * size + h * size + k] = num;
									}

								}
								


							}
						}
						//cur_table[i] = 0;
						empty_space_count[newindex] = count;
					}
				}
			}
			i++;
		}

		index += blockDim.x * gridDim.x;
	}

}

__device__ bool checkValid(int* table, int pos, int val)
{
	if (pos < 0) return checkEntire(table);
	if (val < 1 || val>9) return false;
	int row = pos / size;
	int col = pos % size;
	//check along row, column and cube for the validation of this assignment
	//the start index of coresponding cube 
	for (int h = 0; h < 9; h++)
	{
		//check along row
		if (h != col && table[row * size + h] == val)
		{
			return false;
		}
	}

	for (int h = 0; h < 9; h++)
	{
		//check along column
		if (h != row && table[h * size + col] == val)
		{
			return false;
		}
	}

	//check within a cube
	int i = row - row % 3;
	int j = col - col % 3;
	for (int h = 0; h < 3; h++)
	{
		for (int k = 0; k < 3; k++)
		{
			int idx_r = i + h;
			int idx_c = j + k;
			if (idx_r != row && idx_c != col && table[idx_r * size + idx_c] == val) return false;
		}
	}

	return true;
}

__global__ void sudokuBacktracking(int* table, const int possible_table_counting, int* empty_space, int* empty_space_count, int* finished, int* result_table)
{

	int index = blockIdx.x * blockDim.x + threadIdx.x;

	int* cur_table;
	int* cur_table_empty;
	int num_empty;
	//if the table still has empty spots, keep filling in numbers
	while (((*finished==0)) && (index < possible_table_counting))
	{
		//index of empty space recorded in cur_empty array
		int empty_index = 0;
		cur_table = table + index * size * size;
		cur_table_empty = empty_space + index * size * size;
		num_empty = empty_space_count[index];


		//empty_index = backtracking(cur_table, cur_table_empty, num_empty, empty_index);
		while ((empty_index >= 0) && (empty_index < num_empty))
		{
			cur_table[cur_table_empty[empty_index]]++;

			if (!validBoard(cur_table, cur_table_empty[empty_index])) {
				//validBoard(cur_table, cur_table_empty[empty_index])
				//!checkValid(cur_table, cur_table_empty[empty_index], cur_table[cur_table_empty[empty_index]])
				// if the board is invalid and we tried all numbers here already, backtrack
				// otherwise continue (it will just try the next number in the next iteration)
				if (cur_table[cur_table_empty[empty_index]] >= 9) {
					cur_table[cur_table_empty[empty_index]] = 0;
					empty_index--;
				}
			}
			// if valid board, move forward in algorithm
			else {
				empty_index++;
			}
		}

		//printf("empty_index:%d, num_empty:%d\n", empty_index, num_empty);

		if (empty_index == num_empty)
		{
			*finished =1;
			for (int i = 0; i < size * size; i++)
			{
				result_table[i] = cur_table[i];
			}

			//printf("-------find solution------");
		}

		//*finished = true;
		//for (int i = 0; i < size * size; i++)
		//{
		//	result_table[i] = cur_table[i];
		//}

		index += blockDim.x * gridDim.x;
	}
}

void loadTable(const char* fileName, int* table)
{
	FILE* in = fopen(fileName, "r");
	if (in == NULL)
	{
		printf("File load fail!");
		return;
	}

	char temp;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			fscanf(in, "%c\n", &temp);
			if (temp >= '1' && temp <= '9') table[i * N + j] = (int)(temp - '0');
			else table[i * N + j] = 0;
		}
	}

	//in.close();
}

void printTable(int* table)
{
	printf("-------------------------------\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%d ", table[i * N + j]);
		}
		printf("\n");
	}

	printf("-------------------------------\n");
}

int main(int argc, char* argv[]) {



	const unsigned int threadsPerBlock = atoi(argv[1]);
	const unsigned int maxBlocks = atoi(argv[2]);
	// filename of the starting board
	//const char* filename = "input.txt";
	char* filename = argv[3];
	const char* outfile="result.txt";
	dim3 dimgrid(maxBlocks);
	dim3 dimblock(threadsPerBlock);
	// load the board
	int* board = new int[N * N];
	loadTable(filename, board);

	//alternating used, to store the last round boards and new board generated from last round boards
	int* new_boards;
	int* old_boards;

	//store index of empty space: row*size+col
	int* empty_spaces;
	// stores the number of empty spaces in each board
	int* empty_space_count;
	int* board_index;

	int total_boards = 1;

	//flag for whether solved
	//once equals true: store the table to gpu_solved
	int* finished;
	cudaMalloc(&finished, sizeof(int));
	cudaMemset(finished, 0, sizeof(int));

	//store a solved sudoku table 
	int* gpu_solved;
	cudaMalloc(&gpu_solved, N * N * sizeof(int));
	cudaMemcpy(gpu_solved, board, N * N * sizeof(int), cudaMemcpyHostToDevice);

	//get copied form gpu_solved
	int* solved = new int[N * N];
	memset(solved, 0, N * N * sizeof(int));


	// the size of memory allocation 
	// a experimental number, numbers of new tables will not exceed sk/81
	const int sk = pow(2, 26);

	int host_count;

	//may affect the execution time
	//differnet sudoku puzzle may have differnet optimum execution time 
	// number of iterations to run BFS for
	int iterations =10;

	// allocate memory on the gpu
	cudaMalloc(&empty_spaces, sk * sizeof(int));
	cudaMalloc(&empty_space_count, (sk / 81 + 1) * sizeof(int));
	cudaMalloc(&new_boards, sk * sizeof(int));
	cudaMalloc(&old_boards, sk * sizeof(int));
	cudaMalloc(&board_index, sizeof(int));

	// initialize memory
	cudaMemset(board_index, 0, sizeof(int));
	cudaMemset(new_boards, 0, sk * sizeof(int));
	cudaMemset(old_boards, 0, sk * sizeof(int));

	// copy the initial board to the old boards
	cudaMemcpy(old_boards, board, N * N * sizeof(int), cudaMemcpyHostToDevice);


	struct timespec start, stop;
	double time;
	if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

	//----------------------------debugging--------------------------------------------
	//printf("total table: %d", possible_table_counting);
	//int* firstbfs;
	//int* temp;
	//cudaMalloc(&temp, possible_table_counting*size*size * sizeof(int));
	//cudaMemset(temp, 0, possible_table_counting * size * size * sizeof(int));
	//copytempresult << <dimgrid, dimblock >> > (temp, new_table, possible_table_counting);

	//cudaMemcpy(firstbfs, temp, possible_table_counting *size * size * sizeof(int), cudaMemcpyDeviceToHost);
	//for (int i = 0; i < possible_table_counting * size * size; i++)
	//{
	//	if (i % 9 == 0)
	//	{
	//		printf("\n");
	//	}
	//	if (i % 81 == 0)
	//	{
	//		printf("---------------------------");
	//	}
	//	printf("%d ", firstbfs[i]);
	//}

	//-----------------------------debugging end----------------------------------------------


	// generate more tables based on the initial boards
	//initial_search(int* new_table, int* old_table)
	cudaBFS << <dimgrid, dimblock >> > (old_boards, new_boards, total_boards, board_index, empty_spaces, empty_space_count);

	// loop through BFS iterations to generate more boards deeper in the tree
	for (int i = 0; i < iterations; i++) {
		cudaMemcpy(&host_count, board_index, sizeof(int), cudaMemcpyDeviceToHost);

		printf("total boards after an iteration %d: %d\n", i, host_count);

		cudaMemset(board_index, 0, sizeof(int));


		if (i % 2 == 0) {
			cudaBFS << <dimgrid, dimblock >> > (new_boards, old_boards, host_count, board_index, empty_spaces, empty_space_count);

		}
		else {
			cudaBFS << <dimgrid, dimblock >> > (old_boards, new_boards, host_count, board_index, empty_spaces, empty_space_count);
		}
	}

	cudaMemcpy(&host_count, board_index, sizeof(int), cudaMemcpyDeviceToHost);
	printf("new number of boards retrieved is %d\n", host_count);


	if (iterations % 2 == 1) {
		// if odd number of iterations run, then send it old boards not new boards;
		new_boards = old_boards;
	}

	sudokuBacktracking << <dimgrid, dimblock >> > (new_boards, host_count, empty_spaces, empty_space_count, finished, gpu_solved);
	
	// copy back the solved board

	cudaMemcpy(solved, gpu_solved, N * N * sizeof(int), cudaMemcpyDeviceToHost);


	if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
	time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;
	printf("time is %f s\n", time );


	printTable(solved);

	loadToFile(outfile, solved);

	// free memory
	delete[] board;
	delete[] solved;

	cudaFree(empty_spaces);
	cudaFree(empty_space_count);
	cudaFree(new_boards);
	cudaFree(old_boards);
	cudaFree(board_index);

	cudaFree(finished);
	cudaFree(gpu_solved);

	return 0;

}
