#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

#include <cstdio>
#include <vector>
#include <iostream>
#include "NumberSegmentation.h"
#include "MNISTProcessor.h"
#include "AdaBoostTrainer.h"

#define TRAIN_MODEL false

using namespace std;
using namespace cv;
using namespace cv::ml;

int main() {
	// Step 1. Train AdaBoost Model
	AdaBoostModel adaboost;
	cout << "**** Establish AdaBoost Model ****" << endl;
	string flag = "TRUE, prepare to train AdaBoost Model....";
	if (TRAIN_MODEL == false) {
		flag = "FALSE, load trained AdaBoost Model....";
	}

	cout << "Train Flag: " << flag << endl;
	if (TRAIN_MODEL) {
		adaboost.train();
	}else {
		adaboost.load("output/adaboost.xml");
	}

	cout << "**** Start Testing Single Number Images ****" << endl;
	// Step 2. Test 0 ~ 9 Single number images
	for (int i = 0; i <= 9; i++) {
		string path = "testImg/singleImg/" + to_string(i);
		path += ".png";
		cout << "Test Single Image: " << path << "  ";
		adaboost.predictSingleNumber(path, true);
	}
	
	cout << "Please enter test A4 Image path: ";
	// Step 3. Read input hand-writing-number images and do split segmentation
	// Divide numbers from A4 Paper
	string processImgPath;
    cin >> processImgPath;
    // Open target image
    CImg<int> srcImg(processImgPath.c_str());
    ImageSegmentation ism(srcImg);
    // Start diciding
	cout << "**** Start Image Segmentation ****" << endl;
    ism.processBinaryImage();
    ism.numberSegmentationMainProcess("output/singleNumber");
	// Print middle result
	CImg<int> hist = ism.getHistogramImage();
	hist.display("Histogram Image");
	hist.save("output/divideImgs/hist_2.bmp");

	CImg<int> divide = ism.getImageWithDividingLine();
	divide.display("Divided Line Image");
	divide.save("output/divideImgs/divide_2.bmp");

	CImg<int> win = ism.getNumberDividedCircledImg();
	win.display("Number divide circle");
	win.save("output/divideImgs/circle_2.bmp");

	cout << "**** End Image Segmentation ****" << endl;
	int size = ism.getImageCount();

	// Step 4. Predict number images result and print
	// Test Single number images
	cout << "**** Start A4 Single Number Images Prediction ****" << endl;
	for (int i = 0; i < size; i++) {
		string path = "output/singleNumber/" + to_string(i);
		path += ".bmp";
		cout << "Test A4 Image: " << path << "  ";
		adaboost.predictSingleNumber(path, false);
	}
	cout << "**** End A4 Single Number Images Prediction ****" << endl;
	
	cout << "Press any key to exit...." << endl;
	char pause;
	cin >> pause;

    return 0;
}