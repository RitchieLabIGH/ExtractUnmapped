/*
 * Reduce.cpp
 *
 *  Created on: Feb 7, 2020
 *      Author: claudio
 */

#include "Reduce.h"
using namespace matrix;

int Reduce::run(std::string file_out, double min_acc, double test_perc,
		uint64_t cross_validation) {
	BinaryMatrix mat(matrix_file, true);
	std::vector<uint64_t> groups = mat.groups;
	std::map<uint64_t, uint64_t> group_counts = mat.group_counts;
	std::ofstream ofs(file_out);
	std::vector<KmerMatrixLine> buffer;
	uint64_t bufferSize = 500000, kept = 0, tot_kmers = 0, current_batch=0;
	std::vector<std::string> out_buffer(bufferSize);
	bool running = mat.getBatch(buffer, bufferSize);
	std::cout << "Batch\tPercentageProcessed\tKmerAnalyzed\tKmerKept\n";
	while (running) {
#pragma omp parallel  for schedule(guided, 10)
		for (uint64_t i = 0; i < buffer.size(); i++) {
			std::ostringstream final("");
			std::vector<double> res = MLpack::pairwiseNaiveBayesClassifier( { buffer[i].count }, groups,
					group_counts, cross_validation, test_perc);
			bool keep = false;
			for (double v : res)
				keep = keep || v > min_acc;
			if (keep) {
				final << std::fixed << std::setprecision(3);
				final << buffer[i].getKmer();
				for (double v : res)
					final << "\t" << v;
				final << "\n";
			}
			out_buffer[i] = final.str();
		}
		for (uint64_t i = 0; i < buffer.size(); i++) {
			ofs << out_buffer[i];
			if (out_buffer[i].size() > 1) {
				kept++;
			}
		}
		ofs.flush();
		tot_kmers += buffer.size();
		std::cout << current_batch<< "\t" << mat.perc() << "\t" <<tot_kmers << "\t" << kept  << "\n";
		std::cout.flush();
		current_batch += 1;
		running = mat.getBatch(buffer, bufferSize);
	}
	return 0;
}

