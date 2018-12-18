#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

#include <cstdio>
#include <vector>
#include <iostream>
#include "MNISTProcessor.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

int main() {
    // Start Training
	string dataName = "data/train-images.idx3-ubyte",
		labelName = "data/train-labels.idx1-ubyte";
	// Reading MNIST Images and Labels
	cout << "Start reading MNIST images and labels...." << endl;
    Mat data = readImages(dataName);
    Mat responses = readLabels(labelName);
	cout << "End reading MNIST images and labels...." << endl;
	// Convert Type
	data.convertTo(data, CV_32FC1);
	responses.convertTo(responses, CV_32SC1);

    const int class_count = 10;
    Ptr<Boost> model;
    int nsamples_all = data.rows;
    int ntrain_samples = (int)(nsamples_all * 0.05);
    int var_count = data.cols;
    // Start AdaBoost Training, make Ten class to divide and classify
    Mat new_data( ntrain_samples*class_count, var_count + 1, CV_32FC1 );
    Mat new_responses( ntrain_samples*class_count, 1, CV_32SC1 );
    // Enlarge DataBase Mask Code
    for( int i = 0; i < ntrain_samples; i++ ) {
        const float* data_row = data.ptr<float>(i);
        for( int j = 0; j < class_count; j++ ) {
            float* new_data_row = (float*)new_data.ptr<float>(i*class_count+j);
            memcpy(new_data_row, data_row, var_count*sizeof(data_row[0]));
            new_data_row[var_count] = (float)j;
            new_responses.at<int>(i*class_count + j) = responses.at<int>(i) == j + 0;
        }
    }
    Mat var_type(1, var_count + 2, CV_8U);
    var_type.setTo(Scalar::all(VAR_ORDERED));
    var_type.at<uchar>(var_count) = var_type.at<uchar>(var_count+1) = VAR_CATEGORICAL;
    // Make up train datas
    Ptr<TrainData> tdata = TrainData::create(new_data, ROW_SAMPLE, new_responses, noArray(), noArray(), noArray(), var_type);
    cout << "Start training the classifier...\n"<< endl;
	vector<int> priors;
	priors.push_back(1);
	priors.push_back(10);
    // Create AdaBoost Model
    model = Boost::create();
    // Set AdaBoost Model Parameters
    model->setBoostType(Boost::GENTLE);
    // Weak Classifier Numbers
    model->setWeakCount(50);
    model->setWeightTrimRate(0.95);
    model->setMaxDepth(5);
    model->setUseSurrogates(false);
	model->setPriors(Mat(priors));
    // Train the model
    model->train(tdata);
    // Save the model
    model->save("adaboost.xml" );
	cout << "End training the classifier...\n" << endl;
	
	cout << "Start calculating model correct rate....\n" << endl;
    // Calculate model predict errors
    Mat temp_sample(1, var_count + 1, CV_32F);
    float* tptr = temp_sample.ptr<float>();   
    double train_hr = 0, test_hr = 0;
    for( int i = 0; i < nsamples_all; i++)
    {
        int best_class = 0;
        double max_sum = -DBL_MAX;
        const float* ptr = data.ptr<float>(i);
        for( int k = 0; k < var_count; k++ )
            tptr[k] = ptr[k];
        for( int j = 0; j < class_count; j++ )
        {
            tptr[var_count] = (float)j;
            float s = model->predict( temp_sample, noArray(), StatModel::RAW_OUTPUT );
            if( max_sum < s )
            {
                max_sum = s;
                best_class = j + 0;
            }
        }
        // Judge result true or not (By compare to FLT_EPSILON)
        double r = std::abs(best_class - responses.at<int>(i)) < FLT_EPSILON ? 1 : 0;
        if( i < ntrain_samples )
            train_hr += r;
        else
            test_hr += r;
    }
	cout << "End calculating the model correct rate...." << endl;
    test_hr /= nsamples_all-ntrain_samples;
    train_hr = ntrain_samples > 0 ? train_hr/ntrain_samples : 1.;
    printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n", train_hr*100., test_hr*100.);
    cout << "Number of trees: " << model->getRoots().size() << endl;

	int pause;
	cin >> pause;

    return 0;
}