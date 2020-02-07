/*
 * Reduce.h
 *
 *  Created on: Feb 7, 2020
 *      Author: claudio
 */

#ifndef PROCESS_REDUCE_H_
#define PROCESS_REDUCE_H_


#include "../Matrix/BinaryMatrix.h"
#include "../Utils/MLpack.h"
#include <iomanip>
/*
 *
 */
class Reduce {
public:
	Reduce(){};
	Reduce(std::string input_matrix){
		setMatrix(input_matrix);
	};
	virtual ~Reduce(){};
	void setMatrix(std::string input_matrix){
		matrix_file = input_matrix;
	}
	int run(std::string output, double accuracy_thr, double test_perc,uint64_t cross_val);
private:
	std::string matrix_file;
};

#endif /* PROCESS_REDUCE_H_ */
