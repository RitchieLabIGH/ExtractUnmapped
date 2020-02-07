/*
 * MLpack.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: claudio
 */

#include "MLpack.h"


std::vector<double> MLpack::pairwiseNaiveBayesClassifier(
		const std::vector<std::vector<double>> & values,
		const std::vector<uint64_t> groups,
		const std::map<uint64_t, uint64_t> group_counts,
		const uint64_t crossValidation, double perc_test) {
	classifier cl =
			[](const arma::Mat<double>& trainingData,const arma::Row<size_t>& trainingLabels,const arma::Mat<double>& testData, const arma::Row<size_t>& testLabels) {
				mlpack::naive_bayes::NaiveBayesClassifier<> naiv (trainingData, trainingLabels , 2, false );
				arma::Row<size_t> predictedLabels(testLabels.n_elem);
				naiv.Classify(testData, predictedLabels);
				return accuracy(predictedLabels, testLabels);
			};
	return pairwise_classification(cl, values, groups, group_counts,
			crossValidation,  perc_test);
}

std::vector<double> MLpack::pairwise_classification(
		const classifier classification_fun,
		const std::vector<std::vector<double>> & values,
		const std::vector<uint64_t> groups,
		const std::map<uint64_t, uint64_t> group_counts,
		const uint64_t crossValidation,  double perc_test) {
	arma::Mat<double> trainingData, testData;
	arma::Row<size_t> trainingLabels, testLabels;
	std::vector<double> results;
	uint64_t min_group = 10000;
	for (auto g : group_counts) {
		min_group = g.second < min_group ? g.second : min_group;
	}
	for (uint64_t g = 0; g < group_counts.size(); g++) {
		for (uint64_t h = g + 1; h < group_counts.size(); h++) {
			double sum_of_acc=0;
			for (int cv = 0; cv < crossValidation; cv++) {
				splitTrainingTest(values, groups, trainingData, testData,
						trainingLabels, testLabels, perc_test, min_group, { g,
								h });
				for (int i = 0; i < testLabels.n_elem; i++) {
					if (testLabels(i) == g)
						testLabels(i) = 0;
					else
						testLabels(i) = 1;
				}
				for (int i = 0; i < trainingLabels.n_elem; i++) {
					if (trainingLabels(i) == g)
						trainingLabels(i) = 0;
					else
						trainingLabels(i) = 1;
				}
				sum_of_acc+= classification_fun(trainingData, trainingLabels,
								testData, testLabels);
			}
			results.push_back(sum_of_acc/crossValidation);
		}
	}
	return results;
}


double MLpack::accuracy(const arma::Row<size_t> & prediction,
		const arma::Row<size_t> & real) {
	size_t count = 0;
	for (size_t i = 0; i < prediction.n_elem; i++) {
		if (prediction(i) == real(i))
			count++;
	}
	return (double) (count * 100) / prediction.n_elem;
}

void MLpack::splitTrainingTest(const std::vector<std::vector<double>> & values,
		const std::vector<uint64_t> & groups, arma::Mat<double> & trainingData,
		arma::Mat<double> & testData, arma::Row<size_t> & trainingLabels,
		arma::Row<size_t> & testLabels, double perc_test, uint64_t min_group,
		std::set<uint64_t> groups_to_use) {
	const arma::Col<size_t> order = arma::shuffle(
			arma::linspace<arma::Col<size_t>>(0, groups.size() - 1,
					groups.size()));
	std::map<uint64_t, uint64_t> g_count;
	if (groups_to_use.size() == 0)
		for (auto & el : groups)
			groups_to_use.insert(el);

	uint64_t c_train = 0, c_test = 0, n_rows = values.size(), n_groups =
			groups_to_use.size();
	uint64_t n_test = std::floor(min_group * perc_test);
	if (n_test < 2 && min_group > 5)
		n_test = 2;
	uint64_t n_training = min_group - n_test;
	trainingData.resize(n_rows, n_training * n_groups);
	trainingLabels.resize(n_training * n_groups);
	testData.resize(n_rows, n_test * n_groups);
	testLabels.resize(n_test * n_groups);
	std::cerr.flush();
	for (auto i : order) {
		if (g_count[groups[i]] < min_group
				&& (groups_to_use.count(groups[i]) != 0)) {
			if (g_count[groups[i]] < n_training) {
				trainingLabels(c_train) = groups[i];
				for (int r = 0; r < n_rows; r++) {
					trainingData(r, c_train) = values[r][i];
				}
				c_train++;
			} else {
				testLabels(c_test) = groups[i];
				for (int r = 0; r < n_rows; r++) {
					testData(r, c_test) = values[r][i];
				}
				c_test++;
			}
			g_count[groups[i]]++;
		}
	}
}
