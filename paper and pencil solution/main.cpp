#include"sukudo.h"
#include<fstream>
using namespace std;
sukudo sukudo_now;
void solve() {
	if (sukudo_now.optionlongest <= 1) {
		return;
	}
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_cond_init(&waitCond, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_t* threads = new pthread_t[sukudo_now.optionlongest];
	for (int i = 0; i < sukudo_now.optionlongest; i++) {
		int rc = pthread_create(&threads[i], &attr, distribute_solve, (void*)i);
	}
}

void* sukudo::distribute_solve(void* threadarg) {
	int index = (int)threadarg;
	sukudo temp = *this;
	temp.matrix[longest_i][longest_j].val = temp.matrix[longest_i][longest_j].options_list.at(index);
	bool flag = solve(0, 0, temp);
	if (flag) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				matrix[i][j].val = temp.matrix[i][j].val;
			}
		}
	}
}

int main() {
	ifstream ifs("sukudo.txt");
	int matrix[9][9];
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			int num;
			ifs >> num;
			matrix[i][j] = num;
		}
	}
	sukudo_now = sukudo(matrix);
	sukudo_now.print();

	sukudo_now.findfixedposition();
	solve();
	sukudo_now.print();
	return 0;
}