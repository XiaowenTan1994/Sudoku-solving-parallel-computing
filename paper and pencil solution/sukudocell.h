#pragma once
#include<iostream>
#include<vector>
#include<unordered_map>
#include<unordered_set>
using namespace std;
class sukudocell {
public:
	int val;	
	unordered_set<int> options;
	vector<int> options_list;
	sukudocell() {
		for (int i = 1; i <= 9; i++) {
			options.insert(i);
		}
	}
	void getlist() {
		unordered_set<int> ::iterator itr;
		for (itr = options.begin(); itr != options.end(); itr++) {
			options_list.push_back(*itr);
		}
	}
};