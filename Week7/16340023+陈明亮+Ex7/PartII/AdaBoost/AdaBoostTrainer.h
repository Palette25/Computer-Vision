/*
* AdaBoostTrainer.h -- Make AdaBoost Model and Train
*/
#ifndef ADABOOST_TRAINER_H
#define ADABOOST_TRAINER_H

#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include "MNISTProcessor.h"

#define class_count 10
#define binary_threshold 130

using namespace std;
using namespace cv;
using namespace cv::ml;

class AdaBoostModel{
public:
	AdaBoostModel(){
		this->var_count = 28 * 28;
	}
	void readMNISTDatas();
	void train();
	void predict();
	void load(const string& modelpath);
	int predictSingleNumber(const string& imgPath, bool inverse_flag);

private:
	Ptr<Boost> model;
	Mat data, responses;
	Mat new_data, new_responses;
	// MNIST Training variables
	int nsamples_all, ntrain_samples, var_count;

};

#endif