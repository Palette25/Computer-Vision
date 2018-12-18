#include "AdaBoostTrainer.h"

void AdaBoostModel::readMNISTDatas(){
	string dataName = "data/train-images.idx3-ubyte",
		labelName = "data/train-labels.idx1-ubyte";
	// Reading MNIST Images and Labels
	cout << "Start reading MNIST images and labels...." << endl;
    this->data = readImages(dataName);
    this->responses = readLabels(labelName);
	cout << "End reading MNIST images and labels...." << endl;
	// Convert Type
	data.convertTo(data, CV_32FC1);
	responses.convertTo(responses, CV_32SC1);
}

void AdaBoostModel::train(){
	// Step 1. Read in MNIST Data
	readMNISTDatas();
	// Step 2. Train with MNIST Data
    this->nsamples_all = data.rows;
    this->ntrain_samples = (int)(nsamples_all * 1);
    this->var_count = data.cols;
    // Start AdaBoost Training, make Ten class to divide and classify
    this->new_data = Mat(ntrain_samples * class_count, var_count + 1, CV_32FC1);
    this->new_responses = Mat(ntrain_samples * class_count, 1, CV_32SC1);
    // Enlarge DataBase Mask Code
    for(int i = 0; i < ntrain_samples; i++) {
        const float* data_row = data.ptr<float>(i);
        for(int j = 0; j < class_count; j++) {
            float* new_data_row = (float*)new_data.ptr<float>(i * class_count + j);
            memcpy(new_data_row, data_row, var_count * sizeof(data_row[0]));
            new_data_row[var_count] = (float)j;
            new_responses.at<int>(i * class_count + j) = responses.at<int>(i) == j + 0;
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
    model->setWeakCount(100);
    model->setWeightTrimRate(0.95);
    model->setMaxDepth(5);
    model->setUseSurrogates(false);
	model->setPriors(Mat(priors));
    // Train the model
    model->train(tdata);
    // Save the model
    model->save("output/adaboost.xml" );
	cout << "End training the classifier...\n" << endl;
	// Step 3. Predict accurancy
	predict();
}

void AdaBoostModel::predict(){
	cout << "Start calculating model correct rate....\n" << endl;
    // Calculate model predict errors
    Mat temp_sample(1, var_count + 1, CV_32F);
    float* tptr = temp_sample.ptr<float>();   
    double train_hr = 0, test_hr = 0;
	string dataName = "data/t10k-images.idx3-ubyte",
		labelName = "data/t10k-labels.idx1-ubyte";
	// Reading MNIST Test Images and Labels
	Mat test_data = readImages(dataName);
	Mat test_responses = readLabels(labelName);
	cout << "End reading MNIST images and labels...." << endl;
	// Convert Type
	test_data.convertTo(test_data, CV_32FC1);
	test_responses.convertTo(test_responses, CV_32SC1);
	nsamples_all = test_data.rows;

    for(int i = 0; i < this->nsamples_all; i++){
        int best_class = 0;
        double max_sum = -DBL_MAX;
        const float* ptr = test_data.ptr<float>(i);
        for(int k = 0; k < var_count; k++)
            tptr[k] = ptr[k];
        for(int j = 0; j < class_count; j++){
            tptr[var_count] = (float)j;
            float s = model->predict( temp_sample, noArray(), StatModel::RAW_OUTPUT );
            if(max_sum < s){
                max_sum = s;
                best_class = j + 0;
            }
        }
        // Judge result true or not (By compare to FLT_EPSILON)
        double r = std::abs(best_class - test_responses.at<int>(i)) < FLT_EPSILON ? 1 : 0;
		test_hr += r;
    }
	cout << "End calculating the model correct rate...." << endl;
    test_hr /= this->nsamples_all;
    //train_hr = this->ntrain_samples > 0 ? train_hr / this->ntrain_samples : 1.;

	ofstream out("output/predict.txt");

    printf("Recognition rate: test = %.1f%%\n", test_hr*100.);
	out << "Recognition rate: test = " <<  test_hr*100. << "%\n";
    out << "Number of trees: " << model->getRoots().size() << '\n';

	out.close();
}

void AdaBoostModel::load(const string& modelpath) {
	model = model->load(modelpath);
}

int AdaBoostModel::predictSingleNumber(const string& imgPath, bool flag) {
	Mat srcImg = imread(imgPath.c_str(), 0), result, temp;
	resize(srcImg, srcImg, Size(28, 28));
	if (flag) {
		threshold(srcImg, temp, binary_threshold, 255, THRESH_BINARY_INV);
	}
	else {
		threshold(srcImg, temp, binary_threshold, 255, THRESH_BINARY);
	}
	
	temp.convertTo(result, CV_32FC1);

	Mat Data(1, 28*28, CV_32FC1);

	for (int i = 0; i < 28; i++) {
		for (int j = 0; j < 28; j++) {
			Data.at<float>(i * 28 + j) = result.at<float>(i, j);
		}
	}

	int best_class = -1;
	double max_sum = -DBL_MAX;
	Mat temp_sample(1, var_count + 1, CV_32F);
	float* tptr = temp_sample.ptr<float>();
	const float* ptr = Data.ptr<float>(0);
	for (int k = 0; k < var_count; k++)
		tptr[k] = ptr[k];
	for (int j = 0; j < class_count; j++) {
		tptr[var_count] = (float)j;
		float s = model->predict(temp_sample, noArray(), StatModel::RAW_OUTPUT);
		if (max_sum < s) {
			max_sum = s;
			best_class = j;
		}
	}
	cout << "Best Class: " << best_class << endl;
	return best_class;
}