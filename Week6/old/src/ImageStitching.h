/*
* Program: ImageStitching.cpp
* Usage: Start image stitching
*/
#ifndef IMAGE_STITCHING_H
#define IMAGE_STITCHING_H

#include <iostream>
#include <vector>
#include "SIFT.h"
#include "Utils.h"
#include "Matching.h"
#include "Warping.h"
#include "Blending.h"

class Stitcher{
public:
	Stitcher(vector<CImg<float>> input);
	CImg<float> stitchImages();


private:
	vector<CImg<float>> src;
	
	
};
#endif