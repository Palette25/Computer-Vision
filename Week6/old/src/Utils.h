/*
* Program: Utils.h
* Usgae: Define warping clinder transforma and bilinear_interpolate
*/
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cmath>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Utils{
public:
	Utils(){}
	// Provide tools functions
	float bilinearInterpolate(CImg<float> image, float x, float y, int chanNum);
	double computeSin(double val);
	// Image transform methods
	CImg<float> cylinderizeImg(CImg<float> input);
	CImg<float> toGrayImage(CImg<float> src);
};
#endif