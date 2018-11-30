/*
* Program: Utils.h
* Usgae: Define warping clinder transforma and bilinear_interpolate
*/
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cmath>
#include "CImg.h"

#define PI 3.1415926

using namespace std;
using namespace cimg_library;

class Utils{
public:
	Utils(){}
	// Provide tools functions
	unsigned char bilinearInterpolate(CImg<unsigned char>& image, float x, float y, int chanNum);
	// Image transform methods
	CImg<unsigned char> cylinderizeImg(CImg<unsigned char>& input);
	CImg<unsigned char> toGrayImage(CImg<unsigned char> src);
};
#endif