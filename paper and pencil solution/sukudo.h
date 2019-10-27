#pragma once
#include<iostream>
#include<fstream>
#include"sukudocell.h"
using namespace std;


class sukudo
{
public:
	sukudo() {};
	sukudo(int matrix[9][9]);
	bool checkvalid(int i, int j);
	void print();
	~sukudo();
	sukudocell matrix[9][9];
	int longest_i;
	int longest_j;
	int optionlongest;
};


bool sukudo::checkvalid(int i, int j) {
	// 3 by 3 grid
	for (int index_i = i / 3 * 3; index_i < i / 3 * 3 + 3; index_i++) {
		for (int index_j = j / 3 * 3; index_j < j / 3 * 3 + 3; index_j++) {
			if (index_i == i && index_j == j)
				continue;
			if (matrix[index_i][index_j].val == matrix[i][j].val)
				return false;
		}
	}

	//horizontal
	for (int index_i = 0; index_i < 9; index_i++) {
		if (index_i == i)
			continue;
		if (matrix[index_i][j].val == matrix[i][j].val)
			return false;
	}

	//vertical
	for (int index_j = 0; index_j < 9; index_j++) {
		if (index_j == j)
			continue;
		if (matrix[i][index_j].val == matrix[i][j].val)
			return false;
	}
	return true;
}



void sukudo::print() {
	ofstream ofs("answer.txt");
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			ofs<<matrix[i][j].val;
			ofs << "|";
		}
		ofs<<"\n--------------------\n";
	}
	ofs.close();
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