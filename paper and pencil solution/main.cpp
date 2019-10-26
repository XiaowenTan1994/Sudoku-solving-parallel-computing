#include"sukudo.h"
#include<fstream>
#include<ctime>
#include<pthread.h>
#pragma comment(lib, "pthreadVC2.lib")
using namespace std;
sukudo sukudo_now;
pthread_mutex_t mutex;
pthread_cond_t waitCond1;
pthread_attr_t attr1;
pthread_mutex_t mutex1;
pthread_cond_t waitCond;
pthread_attr_t attr;

void   Delay(int   time);
void* distribute_findfixedposition(void* threadarg);
void* distribute_solve(void* threadarg);
void findfixedposition();
bool solve(int i, int j, sukudo& temp);
void solve();

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
		Delay(3000);
	}
	for (int i = 0; i < sukudo_now.optionlongest; i++) {
		int rc = pthread_join(threads[i],NULL);
	}
	pthread_cond_destroy(&waitCond);
	pthread_mutex_destroy(&mutex1);
}

bool solve(int i, int j, sukudo &temp) {
	if ((temp.matrix[i][j].val != 0)||((i==temp.longest_i)&&(j==temp.longest_j))) {
		j = j + 1;
	}
	if (j >= 9) {
		j = 0;
		i = i + 1;
	}
	if (i >= 9) {
		return true;
	}
	bool flag = true;
	for (int index = 0; index < temp.matrix[i][j].options_list.size(); index++) {
		temp.matrix[i][j].val = temp.matrix[i][j].options_list.at(index);
		if (temp.checkvalid(i, j)) {
			if (solve(i, j + 1, temp)) {
				flag = false;
				break;
			}
		}
	}
	return flag ? false : true;
}

void findfixedposition() {

	pthread_attr_init(&attr1);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_JOINABLE);
	pthread_cond_init(&waitCond1, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_t** threads = new pthread_t*[3];
	for (int i = 0; i < 3; i++) {
		threads[i] = new pthread_t[3];
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			int data[2] = { i,j };
			int rc = pthread_create(&threads[i][j], &attr, distribute_findfixedposition, (void*)data);
		}
	}
	for (int i = 0; i < 3; i++) {
		for(int j =0;j<3;j++){
			int rc = pthread_join(threads[i][j], NULL);
		}
	}
	pthread_cond_destroy(&waitCond);
	pthread_mutex_destroy(&mutex1);
}

void* distribute_solve(void* threadarg) {
	int index = (int)threadarg;
	sukudo temp = sukudo_now;
	temp.matrix[sukudo_now.longest_i][sukudo_now.longest_j].val = temp.matrix[sukudo_now.longest_i][sukudo_now.longest_j].options_list.at(index);
	bool flag = solve(0, 0, temp);
	if (flag) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				sukudo_now.matrix[i][j].val = temp.matrix[i][j].val;
			}
		}
	}
	return (void*)0;
}

void* distribute_findfixedposition(void* threadarg) {
	int *index = (int*)threadarg;
	for (int i = index[0]*3; i < index[0] * 3 + 3; i++) {
		for (int j = index[1] * 3; j < index[1] * 3 + 3; j++) {
			// 3 by 3 grid
			for (int index_i = i / 3 * 3; index_i < i / 3 * 3 + 3; index_i++) {
				for (int index_j = j / 3 * 3; index_j < j / 3 * 3 + 3; index_j++) {
					if (sukudo_now.matrix[i][j].options.find(sukudo_now.matrix[index_i][index_j].val) != sukudo_now.matrix[i][j].options.end()) {
						sukudo_now.matrix[i][j].options.erase(sukudo_now.matrix[index_i][index_j].val);
					}
				}
			}

			//horizontal
			for (int index_i = 0; index_i < 9; index_i++) {
				if (sukudo_now.matrix[i][j].options.find(sukudo_now.matrix[index_i][j].val) != sukudo_now.matrix[i][j].options.end()) {
					sukudo_now.matrix[i][j].options.erase(sukudo_now.matrix[index_i][j].val);
				}
			}

			//vertical
			for (int index_j = 0; index_j < 9; index_j++) {
				if (sukudo_now.matrix[i][j].options.find(sukudo_now.matrix[i][index_j].val) != sukudo_now.matrix[i][j].options.end()) {
					sukudo_now.matrix[i][j].options.erase(sukudo_now.matrix[i][index_j].val);
				}
			}

			if (sukudo_now.matrix[i][j].options.size() == 1) {
				sukudo_now.matrix[i][j].val = *(sukudo_now.matrix[i][j].options.begin());
			}
			sukudo_now.matrix[i][j].getlist();
			// block
			if (sukudo_now.matrix[i][j].options.size() > sukudo_now.optionlongest) {
				sukudo_now.longest_i = i;
				sukudo_now.longest_j = j;
				sukudo_now.optionlongest = sukudo_now.matrix[i][j].options.size();
			}
		}
	}
	return (void*)0;
}

void   Delay(int   time)
{
	clock_t   now = clock();

	while (clock() - now < time);
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

	findfixedposition();
	solve();
	sukudo_now.print();
	return 0;
}