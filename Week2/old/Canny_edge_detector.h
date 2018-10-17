/*
* PROGRAM: Canny_edge_detector.h
* PURPOSE: This header file provides interfaces of "Canny" edge detector.
* The processing steps are as follows:
*
*   1) Convolve the image with a separable gaussian filter.
*   2) Take the dx and dy the first derivatives using [-1,0,1] and [1,0,-1]'.
*   3) Compute the magnitude: sqrt(dx*dx+dy*dy).
*   4) Perform non-maximal suppression.
*   5) Perform hysteresis.
*
* The user must input three parameters. These are as follows:
*	image = The input image file path
*   sigma = The standard deviation of the gaussian smoothing filter.
*   tlow  = Specifies the low value to use in hysteresis. This is a
*           fraction (0-1) of the computed high threshold edge strength value.
*   thigh = Specifies the high value to use in hysteresis. This fraction (0-1)
*           specifies the percentage point in a histogram of the gradient of
*           the magnitude. Magnitude values of zero are not counted in the
*           histogram.
*
*******************************************************************************/
#ifndef CANNY_EDGE_DETECTOR_H
#define CANNY_EDGE_DETECTOR_H

#include <iostream>
#include <vector>
#include <queue>
#include "CImg.h"

#define BOOSTBLURFACTOR 90.0

using namespace cimg_library;
using namespace std;

class Edge_detector {
public:
	Edge_detector(CImg<unsigned char>& image, float sigma, float tlow, float thigh);
	// Startup of the edge detection
	CImg<unsigned char>& edge_detection();
	// Basic Edge detection methods
	void gaussian_smooth();
	vector<vector<double>> make_gaussian_filter(int rows, int cols);
	void sobel();
	void remove_less20_edges();
	// Static usage print methods
	static void usage(char* name);


private:
	CImg<unsigned char> srcImg;
	CImg<unsigned char> edgeImg;
	float sigma, tlow, thigh;
	CImg<float> angles;
};


#endif