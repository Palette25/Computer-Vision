/*
* Program: ImageStitching.cpp
* Usage: Start image stitching
*/
#ifndef IMAGE_STITCHING_H
#define IMAGE_STITCHING_H

#include <vector>
#include <queue>
#include "SIFT.h"
#include "Utils.h"
#include "Matching.h"
#include "Warping.h"
#include "Blending.h"

class Stitcher{
public:
	Stitcher(vector<CImg<unsigned char>> input);
	CImg<unsigned char> stitchImages();
	// Tool functions
	int computeMiddle(vector<vector<int>>& indexes);
	void addFeaturesByHomoegraphy(map<vector<float>, VlSiftKeypoint>& feature, Axis H, float offset_x, float offset_y);
	void adjustOffset(map<vector<float>, VlSiftKeypoint>& feature, int offset_x, int offset_y);

private:
	vector<CImg<unsigned char>> src;
	// Features of every images
	vector<map<vector<float>, VlSiftKeypoint>> features;
	// Tools
	Warper wr;
	Matcher mt;
	Utils ut;
};
#endif