#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int main(){
	ifstream f("output.txt");
	ofstream out("solution.txt");
	string temp;
	vector<int> input;
	vector<int> res;
	vector<int> solution;
	while (getline(f, temp)) {
		input.push_back(stoi(temp));
	}
	for (int num : input) {
		if (num > 0)
			res.push_back(num);
	}
	sort(res.begin(), res.end());
	for (int num : res) {
		int i = num / 81 + 1;
		int j = num / 9 + 1;
		int k = num % 9;
		if (k == 0)
			k = 9;
		solution.push_back(k);
	}
	int row = 0;
	int col = 0;
	temp = "";
	for (int num : solution) {
		col++;
		temp.append(to_string(num)).append(" ");
		if (col == 3 || col == 6)
			temp.append("| ");
		if (col == 9) {
			cout << temp << endl;
			out << temp << endl;
			temp = "";
			col = 0;
			row++;
			if (row == 3 || row == 6) {
				cout << "- - - - - - - - - - -" << endl;
				out << "- - - - - - - - - - -" << endl;
			}
		}
	}
	return 0;
}
