#include <climits>
#include "SlaveWorker.h"
#include "Meter.h"
using namespace std;

extern int CERR_LEVEL;

SlaveWorker::SlaveWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank) {
	stop = false;
    cnf = new CNF(_cnf);
    meta_data_type = _meta_data_type;
    my_rank = _worker_rank;
}

bool SlaveWorker::get_stop() {
	return stop;
}

vector<unsigned> SlaveWorker::encode_variables(VariableSet* variables) {
	unsigned num_assigned = count_assigned(variables);
	vector<unsigned> encoded;
	encoded.reserve(num_assigned);
	vector<Variable*>::iterator iterator;
	for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
		if ((*iterator)->get_assigned()) {
			unsigned encoded_var = (unsigned)(*iterator)->get_name() << 1;
			if ((*iterator)->get_value()) {
				encoded_var++;
			}
			encoded.push_back(encoded_var);
		}
	}
	return encoded;
}

void SlaveWorker::parse_and_update_variables(unsigned encoded[], int size){
	// first "unset" all the variables in this::cnf
	for (auto clause : *cnf->get_clauses()) {
		for (auto v : *clause->get_vars()) {
			v->set_assigned(false);
		}
	}
	// then set them according to encoded[]
	for (auto clause : *cnf->get_clauses()) {
		for (auto v : *clause->get_vars()) {
			for (int i = 0; i < size; i++) {
				int name = encoded[i] >> 1;
				bool encoded_val = encoded[i] % 2 == 1;
				if (v->get_name() == name) {
					v->set_assigned(true);
					v->set_value(encoded_val);
				}
			}
		}
	}
}

unsigned SlaveWorker::count_assigned(VariableSet* variables) {
	unsigned num_assigned = 0;
	for (auto var : *variables) {
		if (var->get_assigned()) {
			num_assigned++;
		}
	}
	return num_assigned;
}

void SlaveWorker::send_measurements() {
	stop_runtime();
	unsigned data[4];
	data[0] = get_runtime();
	data[1] = get_waiting_time();
	data[2] = get_send_messages();
	data[3] = get_send_meta();
	MPI_Send(data, 4, MPI_UNSIGNED, 0, 2, MPI_COMM_WORLD);
}

//void SlaveWorker::set_config(Config *conf) {
  //  config = conf;
//}

unsigned count_assigned(VariableSet *variables) {
    unsigned num_assigned = 0;
    VariableSet::iterator iterator;
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            num_assigned++;
        }
    }
    return num_assigned;
}

/**
 * runs cdcl on the cnf store in this::cnf
 */
void SlaveWorker::run_cdcl() {
    CDCL *cdcl = new CDCL(cnf);
    bool sat = cdcl->SATISFIABLE();
    if (sat) {
        send_sat(cdcl->get_cnf());
    } else {
        send_unsat();
    }
}

/**
 * check if we receive a stop message from master before the messages are delivered to master
 * if that's the case, cancel the message sends and return true
 * -> the caller is then responsible to * stop.
 */
bool SlaveWorker::stop_received_before_message_completion(MPI_Request *mpi_requests, int size) {
    int flag = 0;
    int all_done = 0;
    if (!this->stop) {
        while (flag == 0 && all_done == 0) {
            MPI_Testall(size, mpi_requests, &all_done, MPI_STATUS_IGNORE);
            MPI_Iprobe(0, 1, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
        }
    }
    if (flag != MPI_SUCCESS && !this->stop) {
        struct meta meta;
        MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        inc_recv_messages(sizeof(struct meta));
        this->stop = true;
        if (!all_done) {
            for (int i = 0; i < size; i++) {
                MPI_Cancel(mpi_requests + i);
            }
        }
        send_measurements();
    }
    return flag != 0;
}

/**
 * callback that is used on a dpll branch
 * @param variables contains the variable assignments that some other worker should try
 */
void SlaveWorker::dpll_callback(VariableSet *variables) {
    unsigned num_assigned = count_assigned(variables);
    MPI_Request mpi_requests[2];
    mpi_requests[0] = send_meta(10, num_assigned);
    mpi_requests[1] = send_model(encode_variables(variables));
    bool stop = stop_received_before_message_completion(mpi_requests, 2);
    if (stop) {
        this->stop = true;
    }
}

/**
 * Sends meta data to master (non-blocking send)
 * @param i message type
 * @param assigned the number of assigned variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request SlaveWorker::send_meta(char i, unsigned assigned) {
    struct meta meta;
    meta.message_type = i;
    meta.count = assigned;

    inc_send_messages(sizeof(struct meta));
    inc_send_meta_cout();

    MPI_Request request;
    MPI_Isend(&meta, 1, meta_data_type, 0, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends model to master (non-blocking send)
 * @param assigned the vector of encoded variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request SlaveWorker::send_model(std::vector<unsigned> assigned) {
    inc_send_messages(assigned.size() * sizeof(unsigned));

    MPI_Request request;
    MPI_Isend(&assigned.front(), (int) assigned.size(), MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends sat meta data and satisfiable model to master, triggers waiting for further instructions
 * @param cnf object that contains the assigned variables
 */
void SlaveWorker::send_sat(CNF *cnf) {
    if (!this->stop) {
        VariableSet *vars = cnf->get_model();
        // go through the variables and assign true to all the unassigned ones
        for (auto v : *vars) {
            if(!v->get_assigned()) {
                v->set_assigned(true);
                v->set_value(true);
            }
        }
        unsigned num_assigned = count_assigned(vars);
        MPI_Request requests[2];
        requests[0] = send_meta(12, num_assigned);
        requests[1] = send_model(encode_variables(vars));

        bool stop_received = stop_received_before_message_completion(requests, 2);
        if (!stop_received) {
            wait_for_instructions_from_master();
        }
    }
}

/**
 * Sends unsat result to master and triggers waiting for further instructions
 */
void SlaveWorker::send_unsat() {
    if (!this->stop) {
        start_waiting();
        MPI_Request request = send_meta(11, 0);
        bool stop_received = stop_received_before_message_completion(&request, 1);
        if (!stop_received) {
            wait_for_instructions_from_master();
        }
    }
}

/**
 * Enters a blocking receive on messages from master
 * Handles received messages:
 * - either done message -> stop
 * - or more work message -> setup variables start dpll
 */
void SlaveWorker::wait_for_instructions_from_master() {
    struct meta meta;
    MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    inc_recv_messages(sizeof(struct meta));

    if (meta.message_type == 0) {
        stop_waiting();
        unsigned *encoded_model = new unsigned[meta.count];
        if (meta.count > 0) {
            MPI_Recv(encoded_model, meta.count, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            inc_recv_messages(meta.count * sizeof(unsigned));
            parse_and_update_variables(encoded_model, meta.count);
        } else {
            unsigned encoded[1];
            parse_and_update_variables(encoded, 0);
        }
        run_cdcl();
    } else {
        send_measurements();
    }
}
