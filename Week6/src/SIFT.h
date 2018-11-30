/*
* Program: SIFT.h
* Usage: Define features patching methods
*/
#ifndef SIFT_H
#define SIFT_H

#include <iostream>
#include <vector>
#include <map>
#include "CImg.h"

using namespace std;
using namespace cimg_library;
// Import and use vlfeat for sift features scanning
extern "C"{
	#include "vl/generic.h"
	#include "vl/sift.h"
}

class SIFT{
public:
	SIFT(CImg<float> src);
	map<vector<float>, VlSiftKeypoint> getSiftFeatures();
	void drawSiftFeatires(float fac);

private:
	CImg<float> src;
	map<vector<float>, VlSiftKeypoint> Features;

};

#endif