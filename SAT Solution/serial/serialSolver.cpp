#include <string>
#include <fstream>
#include <chrono>
#include <map>
#include <stdexcept>
#include "Variable.h"
#include "Clause.h"
#include "Cnf.h"
#include "CnfParser.h"
#include "Cdcl.h"

using namespace std;
using namespace std::chrono;

void default_args(map<string, string>* arg_map) {
	arg_map->insert({ "-s", "CDCL" });
};

int main(int argc, char* argv[]) {
	map<string, string> arg_map;
	default_args(&arg_map);

	auto* parser = new CNFParser(argv[argc - 1]);
	parser->parsing();
	unordered_set<CNF*>::iterator cur;
	unordered_set<CNF*> cnfs = parser->get_CNFS();

	// timestamp starter
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for (cur = cnfs.begin(); cur != cnfs.end(); cur++) {
		auto* res = new CDCL(*cur);
		bool sat = res->SATISFIABLE();
		if (sat) {
			CDCL::output(res->get_cnf()->get_model());
		}
		else {
			cout << "unsat" << std::endl;
		}
		delete res;
	}

	// time end
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << "Time to solve SAT in serial: " <<duration << endl;
	return 0;
}
