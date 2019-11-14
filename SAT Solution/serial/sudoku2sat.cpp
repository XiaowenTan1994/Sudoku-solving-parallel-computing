#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void readFile() {
	vector<string> res;
	string puzzle;
	ifstream f("input.txt");
	ofstream out("input.cnf");
	int count = 0;
	int total = 0;

	for (int row = 0; row < 9; row++) {
		f >> puzzle;
		for (int col = 0; col < 9; col++) {
			if (puzzle[col] != '0') {
				int num = 81 * row + 9 * col + puzzle[col] - '0';
				res.push_back(to_string(num) + " 0" + "\n");
				count++;
			}
		}
	}
	f.close();
	total = 8829 + count;
	out << "p cnf " << "729 " << to_string(total) << endl;
	for (string& s : res) {
		out << s;
	}
	out.close();
}

void cell() {
	string res;
	ofstream out;
	out.open("input.cnf", ofstream::app);
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				int num = 81 * i + 9 * j + k + 1;
				res += to_string(num);
				res += " ";
			}
			out << res << "0" << endl;
			res.clear();
		}
	}
	out.close();
}

void row() {
	ofstream out;
	out.open("input.cnf", ofstream::app);
	for (int i = 0; i < 9; i++) {
		for (int k = 0; k < 9; k++) {
			for (int j = 0; j < 8; j++) {
				for (int l = j + 1; l < 9; l++) {
					int first = -(81 * i + 9 * j + k + 1);
					int second = -(81 * i + 9 * l + k + 1);
					out << first << " " << second << " 0" << endl;
				}
			}
		}
	}
	out.close();
}

void column() {
	ofstream out;
	out.open("input.cnf", ofstream::app);
	for (int j = 0; j < 9; j++) {
		for (int k = 0; k < 9; k++) {
			for (int i = 0; i < 8; i++) {
				for (int l = i + 1; l < 9; l++) {
					int first = -(81 * i + 9 * j + k + 1);
					int second = -(81 * l + 9 * j + k + 1);
					out << first << " " << second << " 0"<< endl;
				}
			}
		}
	}
	out.close();
}

void threeSquare() {
	ofstream out;
	out.open("input.cnf", ofstream::app);
	for (int k = 0; k < 9; k++) {
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 3; b++) {
				for (int u = 0; u < 3; u++) {
					for (int v = 0; v < 2; v++) {
						for (int w = v + 1; w < 3; w++) {
							int first = -(81 * (3 * a + u) + 9 * (3 * b + v) + k + 1);
							int second = -(81 * (3 * a + u) + 9 * (3 * b + w) + k + 1);
							out << first << " " << second << " 0" << endl;
						}
					}
				}
			}
		}
	}
	for (int k = 0; k < 9; k++) {
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 3; b++) {
				for (int u = 0; u < 2; u++) {
					for (int v = 0; v < 3; v++) {
						for (int w = u + 1; w < 3; w++) {
							for (int t = 0; t < 3; t++) {
								int first = -(81 * (3 * a + u) + 9 * (3 * b + v) + k + 1);
								int second = -(81 * (3 * a + w) + 9 * (3 * b + t) + k + 1);
								out << first << " " << second << " 0" << endl;
							}
						}
					}
				}
			}
		}
	}
}

int main(){
	readFile();
	cell();
	row();
	column();
	threeSquare();
	return 0;
}

