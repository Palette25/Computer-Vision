/*
*  HoughTransform.h -- Perform hough transform
*/
#ifndef HOUGH_TRANSFORM_H
#define HOUGH_TRANSFORM_H

#include "utils.h"

#define DIVIDER 180
#define MAX 0x7FFFFFFF

class HoughTransformer {
public:
	HoughTransformer(double threshold, double peakDis, CImg<unsigned char>& input, double fitDis, bool flag);
	CImg<int> initHoughSpace();
	void findPeaks(double a, double b);
	// Getter methods
	void getLines();
	void getIntersactions();
	CImg<unsigned char> getResult(){
		return this->intersactionImg;
	}

	double theta;
	vector<HoughPos> result;

private:
	CImg<int> houghSpace;
	// Basic Image store
	CImg<unsigned char> edgeImg;
	CImg<unsigned char> lineImg;
	CImg<unsigned char> intersactionImg;
	// Other Variables
	vector<HoughPos> peaks;

};

#endif