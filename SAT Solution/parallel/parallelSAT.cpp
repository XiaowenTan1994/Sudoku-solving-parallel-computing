#include <iostream>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <mpi.h>
#include "CnfParser.h"
#include "SlaveWorker.h"
#include "Master.h"
#include "ArgParsing.h"

using namespace std;
using namespace std::chrono;

int CERR_LEVEL = 0;

int main(int argc, char* argv[]) {
	char* path = argv[argc - 1];
	map<std::string, std::string> arg_map;
	parse_args(&arg_map, argc, argv);

	CNFParser* parser = new CNFParser(path);
	parser->parsing();
	unordered_set<CNF*> cnfs = parser->get_CNFS();
	CNF cnf = *(*(cnfs.begin())); 

	MPI_Init(&argc, &argv);
	MPI_Datatype meta_data_type = setup_meta_type();
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0) {
		Master* master = new Master((size_t)size, 0, meta_data_type, path);
		master->start();
		while (!master->listen_to_workers());
		delete master;
	}
	else {
		SlaveWorker* w = new SlaveWorker(*(*(cnfs.begin())), meta_data_type, rank);
		w->wait_for_instructions_from_master();
		delete w;
	}

	MPI_Finalize();       
	return EXIT_SUCCESS;
}
