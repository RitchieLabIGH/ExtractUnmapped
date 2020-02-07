/*
 * MLpack.h
 *
 *  Created on: Feb 11, 2019
 *      Author: claudio
 */

#ifndef MACHINELEARNING_MLPACK_H_
#define MACHINELEARNING_MLPACK_H_

#include <map>
#include <vector>
#include <set>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/core/data/split_data.hpp>

typedef const std::function<double(const arma::Mat<double>&,const arma::Row<size_t>&, const arma::Mat<double>&,const arma::Row<size_t>& )> classifier;

///Interface for the MLPack library : http://mlpack.org/doc/mlpack-3.1.1/doxygen/index.html
///
class MLpack {
public:
	MLpack(){};
	virtual ~MLpack(){};
	static std::vector<double> pairwiseNaiveBayesClassifier(const std::vector<std::vector<double>> & values,const std::vector<uint64_t> groups,const std::map<uint64_t, uint64_t>  group_counts, const uint64_t crossValidation, const double perc_train);
	static double accuracy(const arma::Row<size_t> & prediction,const arma::Row<size_t> & real);
	static void splitTrainingTest(const std::vector<std::vector<double>> & values,
			const std::vector<uint64_t> & groups, arma::Mat<double> & trainingData,
			arma::Mat<double> & testData, arma::Row<size_t> & trainingLabels,
			arma::Row<size_t> & testLabels, double perc_train, uint64_t min_group, std::set<uint64_t> groups_to_use);
private:
	static std::vector<double> pairwise_classification(const classifier classification_function , const std::vector<std::vector<double>> & values,const std::vector<uint64_t> groups,const std::map<uint64_t, uint64_t>  group_counts, const uint64_t crossValidation,  const double perc_train);
};
#endif /* MACHINELEARNING_MLPACK_H_ */
