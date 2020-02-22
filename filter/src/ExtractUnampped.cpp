//============================================================================
// Name        : ExtractUnampped.cpp
// Author      : Claudio Lorenzi ( claudio.lorenzi@igh.cnrs.fr )
// Version     : v0.1
// Copyright   :  
//    This software is governed by the CeCILL  license under French law and
//    abiding by the rules of distribution of free software.  You can  use, 
//    modify and/ or redistribute the software under the terms of the CeCILL
//    license as circulated by CEA, CNRS and INRIA at the following URL
//                       "http://www.cecill.info". 
// 
//    As a counterpart to the access to the source code and  rights to copy,
//    modify and redistribute granted by the license, users are provided only
//    with a limited warranty  and the software's author,  the holder of the
//    economic rights,  and the successive licensors  have only  limited
//    liability. 
//
//    In this respect, the user's attention is drawn to the risks associated
//    with loading,  using,  modifying and/or developing or reproducing the
//    software by the user in light of its specific status of free software,
//    that may mean  that it is complicated to manipulate,  and  that  also
//    therefore means  that it is reserved for developers  and  experienced
//    professionals having in-depth computer knowledge. Users are therefore
//    encouraged to load and test the software's suitability as regards their
//    requirements in conditions enabling the security of their systems and/or 
//    data to be ensured and,  more generally, to use and operate it in the 
//    same conditions as regards security. 
//
//    The fact that you are presently reading this means that you have had
//    knowledge of the CeCILL license and that you accept its terms.
//
//============================================================================

#include <iostream>
#include "Utils/cxxopts.hpp"
#include "Process/Reduce.h"


std::string help(){
	return "ExtractUnmapped matrix|reduce|help\n\t - matrix : generate a matrix from a list of k-mer counts in tsv format\n\t - reduce : reduce a matrix to keep the k-mer able to discriminate the samples's classes.\n\t - help : show this help.";
}

int main(int argc, char** argv) {
	std::string action = "help";
	if (argc > 1) {
		action = argv[1];
	}
	if (action != "matrix" && action != "reduce" ) {
		std::cout << help() << "\n";
		return 0;
	}
	if (action == "matrix"){
		cxxopts::Options options("ExtractUnmapped matrix",
					"Convert text files in binary to be used in the reduction step");
			options.add_options()("i,input", "The input file in tsv format containing the location of the sorted k-mer counts",
					cxxopts::value<std::string>())("o,output", "Output matrix file, in json format",
					cxxopts::value<std::string>())("r,rescale-factor", "Rescale the counts to avoid precision problems",
							cxxopts::value<double>()->default_value("1e9"))("h,help", "Show this help");
			auto parsedArgs = options.parse(argc, argv);
			if ( parsedArgs.count("help") != 0){
				std::cerr <<  options.help() << "\n";
				return 0;
			}
			matrix::BinaryMatrix bm;
			bm.create(parsedArgs["input"].as<std::string>(),parsedArgs["rescale-factor"].as<double>(), -1 );
			bm.save(parsedArgs["output"].as<std::string>());
			return 0;
	} else {
		cxxopts::Options options("ExtractUnmapped reduce",
					"Reduce a k-mer matrix in according to the classification power of each k-mer");
			options.add_options()("i,input", "The input matrix in JSON format produced using the matrix action.",
					cxxopts::value<std::string>())("o,output", "Output matrix file",
					cxxopts::value<std::string>())("h,help", "Show this help")(
					"a,accuracy", "Minimum of accuracy",
					cxxopts::value<double>()->default_value("65"))("t,test-percentage",
					"The percentage of the min class used as test size",
					cxxopts::value<double>()->default_value("0.25"))(
					"c,cross-validation", "Number of cross validation",
					cxxopts::value<uint64_t>()->default_value("10"));
			auto parsedArgs = options.parse(argc, argv);
			if ( parsedArgs.count("help") != 0){
				std::cerr <<  options.help() << "\n";
				return 0;
			}
			Reduce process(parsedArgs["input"].as<std::string>());
			return process.run(parsedArgs["output"].as<std::string>(), parsedArgs["accuracy"].as<double>(),
					parsedArgs["test-percentage"].as<double>(),parsedArgs["cross-validation"].as<uint64_t>());
	}

}
