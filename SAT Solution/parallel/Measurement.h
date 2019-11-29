#ifndef SAT_SOLVER_MEASUREMENT_H
#define SAT_SOLVER_MEASUREMENT_H


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const int NUM_MEASUREMENTS = 4;

class Measurement {
private:
    std::vector<std::vector<unsigned>> data;
public:
    void add_measurement(std::vector<unsigned> worker_data) {
        data.push_back(worker_data);
    }

    void write_to_files() {
	std::cout << "Execution time: " << data[0][0] / (data.size() - 1) << std::endl;
    }

    explicit Measurement(std::string cnf_path, std::string prefix) {
    }
};


#endif //SAT_SOLVER_MEASUREMENT_H
