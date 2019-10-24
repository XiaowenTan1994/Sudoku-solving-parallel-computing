#pragma once
#include<iostream>
#include<fstream>
#include<pthread.h>
#include"sukudocell.h"
using namespace std;
pthread_mutex_t mutex1;
pthread_cond_t waitCond;
pthread_attr_t attr;

class sukudo
{
public:
	sukudo(int matrix[9][9]);
	void findfixedposition();
	bool checkvalid(int i, int j);
	void print();
	void* distribute_solve(void *threadarg);
	~sukudo();
	sukudocell matrix[9][9];
	int longest_i;
	int longest_j;
	int optionlongest;
};



void sukudo::findfixedposition() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			// 3 by 3 grid
			for (int index_i = i / 3 * 3; index_i < i / 3 * 3 + 3; index_i++) {
				for (int index_j = j / 3 * 3; index_j < j / 3 * 3 + 3; index_j++) {
					if (matrix[i][j].options.find(matrix[index_i][index_j].val) != matrix[i][j].options.end) {
						matrix[i][j].options.erase(matrix[index_i][index_j].val);
					}
				}
			}

			//horizontal
			for (int index_i = 0; index_i < 9; index_i++) {
				if (matrix[i][j].options.find(matrix[index_i][j].val) != matrix[i][j].options.end) {
					matrix[i][j].options.erase(matrix[index_i][j].val);
				}
			}

			//vertical
			for (int index_j = 0; index_j < 9; index_j++) {
				if (matrix[i][j].options.find(matrix[i][index_j].val) != matrix[i][j].options.end) {
					matrix[i][j].options.erase(matrix[i][index_j].val);
				}
			}

			if (matrix[i][j].options.size == 1) {
				matrix[i][j].val = *(matrix[i][j].options.begin());
			}
			matrix[i][j].getlist();
			// block
			if (matrix[i][j].options.size() > optionlongest) {
				longest_i = i;
				longest_j = j;
				optionlongest = matrix[i][j].options.size();
			}
		}
	}
}


void sukudo::print() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << matrix[i][j].val << '|';
		}
		cout << endl << "--------------------" << endl;
	}
}


bool sukudo::solve(int i, int j, sukudo temp) {
	
}

sukudo::sukudo(int data[9][9])
{
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			matrix[i][j].val = data[i][j];
		}
	}
	longest_i = 0;
	longest_j = 0;
	optionlongest = 0;
}

sukudo::~sukudo()
{
}