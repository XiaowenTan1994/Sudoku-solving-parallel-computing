#ifndef SAT_SLAVE_SOLVER_WORKER_H
#define SAT_SLAVE_SOLVER_WORKER_H

#include "Cnf.h"
#include "Cdcl.h"
#include "Meter.h"
#include "MpiTypes.h"
#include <stdexcept>
#include <vector>
#include "InternalTypes.h"

class SlaveWorker : public Meter {
private:
	CNF* cnf;                                                                               // parsed cnf
	MPI_Datatype meta_data_type;                                                            // MPi datatype used for communication
	int my_rank;                                                                            // rank of process working is running on
	bool stop;
    MPI_Request send_meta(char i, unsigned assigned);
    MPI_Request send_model(std::vector<unsigned int> assigned);   
    void send_sat(CNF *cnf);   
    void send_unsat();   
	bool stop_received_before_message_completion(MPI_Request* mpi_requests, int size);
	void run_cdcl();
    //Config *config;
    
public:
	void send_measurements();
	unsigned count_assigned(VariableSet* variables);
	std::vector<unsigned> encode_variables(VariableSet* variables);
	void parse_and_update_variables(unsigned[], int size);
	bool get_stop();

    explicit SlaveWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank);
    //void set_config(Config *conf);
    virtual void dpll_callback(VariableSet *variables);  
    void wait_for_instructions_from_master();
};


#endif //SAT_SLAVE_SOLVER_WORKER_H
