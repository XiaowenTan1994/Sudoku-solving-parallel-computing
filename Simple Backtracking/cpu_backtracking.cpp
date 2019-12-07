//serial sudoku solver--backtracking
//author: Siqi Chen
#include<iostream>
#include <vector>
#include<fstream>

using namespace std;
#define size 9


void loadTable(const char* fileName, vector<vector<int>>& table)
{
	ifstream in;
	in.open(fileName);
	if (!in)
	{
		cout << "file load fail!" << endl;
		return;
	}

	char temp;
	for (int i = 0; i < size; i++)
	{
		vector<int> line;
		for (int j = 0; j < size; j++)
		{
			in >> temp;
			if (temp >= '1' && temp <= '9') line.push_back((int)(temp - '0'));
			else line.push_back(0);
		}
		table.push_back(line);
	}

	in.close();

}

void printTable(vector<vector<int>> &table)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cout << table[i][j]<<" ";
		}
		cout << endl;
	}
}

bool checkValid(vector<vector<int>> & table, int i, int j, int val)
{
	//check along row, column and cube for the validation of this assignment
	//the start index of coresponding cube 
	int row = i - i % 3, col = j - j % 3;
	for (int h = 0; h < 9; h++)
	{
		//check along row
		if (table[h][j] == val)
		{
			return false;
		}
	}
	for (int h = 0; h < 9; h++)
	{
		//check along column
		if (table[i][h] == val)
		{
			return false;
		}
	}

	//check within a cube
	for (int h = 0; h < 3; h++)
	{
		for (int k = 0; k < 3; k++)
		{
			if (table[row + h][col + k] == val) return false;
		}
	}

	return true;
}

bool solveSudoku(vector<vector<int>> & table, int i, int j)
{
	//traverse along the line 
	if (i == 9) return true;
	if (j == 9) return solveSudoku(table, i + 1, 0);
	if (table[i][j] != 0) return solveSudoku(table, i , j+1);

	for (int c = 1; c <= 9; c++)
	{
		if (checkValid(table, i, j, c))
		{
			table[i][j] = c;
			if (solveSudoku(table, i, j + 1)) return true;
			table[i][j] = 0;
		}
	}

	return false;
}

void solveSudoku(vector<vector<int>> &table)
{
	solveSudoku(table, 0, 0);
}


void loadToFile(const char* outFile, vector<vector<int>> &table)
{
	ofstream out;
	out.open(outFile);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			out << table[i][j] << " ";
		}
		out << endl;
	}

	out.close();
}


bool checkValid(int* table, int pos, int val)
{
	int row = pos / size;
	int col = pos % size;
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

void backtracking(int* cur_table, int* cur_table_empty, int num_empty, int empty_index)
{
	if (empty_index < 0 || empty_index >= num_empty) return;
	int position = cur_table_empty[empty_index];
	for (int i = 1; i <= 9; i++)
	{
		if (checkValid(cur_table, position, i))
		{
			cur_table[position] = i;
			backtracking(cur_table, cur_table_empty, num_empty, empty_index + 1);
		}
	}
	

}


int main(int argc, char* argv[])
{
	vector<vector<int>> table;

	char* filename = argv[1];
	string result = "result.txt";
	loadTable(file.c_str(), table);
	printTable(table);

	struct timespec start, stop;
	double time;
	if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

	solveSudoku(table);

	if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
	time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;
	printf("time is %f s\n", time);

	printTable(table);
	loadToFile(result.c_str(), table);
	//while (1);

	//int arr[] = { 2,6,1,3,7,5,8,9,4,5,3,7,8,9,4,1,0,0,0,4,8,2,1,6,3,5,7,0,9,0,7,5,1,2,3,8,8,2,5,9,4,3,6,7,1,7,1,3,6,2,8,9,4,5,
	//	0,8,9,1,6,0,5,2,3,
	//	1,0,2,0,3,9,4,8,0,
	//	3,0,6,4,0,2,7,1,9 };
	//int* table = arr;
	//int arr2[] = { 16,17,18,27,29,54,59,64,66,71,73,76 };
	//int* cur_table_empty = arr2;
	//int num_empty = 12;
	//int empty_index = 0;
	//backtracking(table, cur_table_empty, num_empty, empty_index);


	//for (int i = 0; i < 81; i++)
	//{
	//	if (i % 9 == 0)
	//	{
	//		cout << endl;
	//		cout << table[i] << " ";
	//	}
	//	else
	//	{
	//		cout << table[i] << " ";
	//	}
	//}
	//while (1);
	//return 0;

}

