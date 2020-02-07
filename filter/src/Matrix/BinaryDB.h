/*
 * BinaryDB.h
 *
 *  Created on: Feb 8, 2019
 *      Author: claudio
 */

#ifndef MATRIX_BINARYDB_H_
#define MATRIX_BINARYDB_H_

#include "Kmer.h"

namespace matrix {

class BinaryDB {
public:
	BinaryDB() {
	}
	;
	BinaryDB(std::string file) {
		open(file);
	}
	;

	static bool create(std::string file, int64_t prefix_size);
	bool open(std::string file);
	void close() {
		tot_suffix = 0;
	}
	virtual ~BinaryDB();
	bool getNext();
	std::vector<double> getKmers(std::vector<Kmer> & requests);

	void print(std::ofstream & of) {
		of << getKmer() << "\t" << getCount() << "\n";
		while (getNext()) {
			of << getKmer() << "\t" << getCount() << "\n";
		}
	}
	bool isOpen() {
		return current_suffix != 0  && current_suffix <= tot_suffix;
	}
	;
	double perc() {
		return tot_suffix == 0 ? 100 :  ((current_suffix) * 100 / tot_suffix);
	}
	;
	void setBufferSize(uint64_t bs) {
		if (isOpen()) {
			std::cerr
					<< "ERROR: close the binaryDB before change the buffer size!\n";
		} else {
			buffer_size = bs;
		}

	}
	uint64_t getTotCount() {
		return tot_count;
	}
	;
	uint64_t getTotPrefix() {
		return tot_prefix;
	}
	;
	uint64_t getTotSuffix() {
		return tot_suffix;
	}
	;
	int64_t getPrefixSize() {
		return prefix_size;
	}
	;

	Kmer & getKmer() {
		return kmer;
	}
	Suffix & getSuffix() {
		return suffix;
	}
	Prefix & getPrefix() {
		return current_prefix_range.first;
	}
	uint32_t & getCount() {
		return suffix.count;
	}

	bool go_to(Kmer & );
	std::string file_name;
	static int64_t bestPrefixSize(std::string file);
	uint32_t query_count;
	std::pair<Prefix, std::pair<int64_t, int64_t>> current_prefix_range;
	uint64_t current_suffix = 0;

private:
	int64_t prefix_size;
	int64_t suffix_size;
	/// The pointers
	int64_t prefix_curr_p = 0;
	int64_t prefix_init_p = 0;
	int64_t suffix_init_p = 0;

	/// The binary dimensions
	int64_t unit_suffix_binary;
	int64_t unit_prefix_binary;
	uint64_t prefix_v_size = 0;
	uint64_t suffix_v_size = 0;
	uint64_t prefix_v_byte_size = 0;
	uint64_t suffix_v_byte_size = 0;
	uint64_t c_size = sizeof(uint32_t);
	uint64_t p_size = sizeof(uint64_t);

	Suffix suffix;
	Kmer kmer;
	uint64_t buffer_size = 1000000;
	uint64_t buffer_prefix_size;
	uint64_t tot_prefix = 0;
	uint64_t tot_suffix = 0;

	uint64_t current_prefix = 0;
	uint64_t buffer_p = 0;
	uint64_t p_buffer_p = 0;
	uint64_t tot_count = 0;

	FILE *file = NULL;
	FILE *file_prefix = NULL;
	std::vector<uchar> stream_buffer;
	std::vector<uchar> stream_buffer_prefix;

	void fillBuffer();
	void fillPrefixBuffer();
	bool getNextPrefix();
	bool hybrid_binary_search( Kmer & target, std::vector<uchar> & prefixes, std::vector<uchar> & stream_buffer);
	void loadPrefixes(std::vector<uchar> & prefixes);
	void loadSuffixes(std::vector<uchar> & suffixes);
};


static uint64_t countFileLines(std::string file) {
	std::ifstream fin(file.c_str());
	if (!fin) {
		std::cerr << "countFileLines: Error while opening " << file
				<< " in read mode" << std::endl;
		exit(EXIT_FAILURE);
	}
	uint64_t count = 0;
	const int SZ = 1024 * 1024;
	std::vector<char> buff(SZ);
	fin.read(&buff[0], buff.size());
	const char * p;
	while (int cc = fin.gcount()) {
		p = &buff[0];
		for (int i = 0; i < cc; i++) {
			if (p[i] == '\n')
				count++;
		}
		fin.read(&buff[0], buff.size());
	}
	fin.close();
	return count;
}


static std::string getLineFromFile(std::string fname, uint64_t lineN) {
	std::ifstream inFile(fname);
	if (!inFile.good()) {
		std::cerr << "Error! Problem opening " << fname;
		exit(1);
	}
	std::string line;

	int ind = 0;
	getline(inFile, line);
	while (ind != lineN) {
		getline(inFile, line);
		ind++;
	}
	inFile.close();
	return line;
}

}
#endif /* MATRIX_BINARYDB_H_ */
