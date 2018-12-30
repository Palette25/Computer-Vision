/*
*  ImageSegment.h -- For Basic Input A4 Paper segmentation
*/
#ifndef IMAGE_SEGMENT_H
#define IMAGE_SEGMENT_H

#include "utils.h"

class ImageSegmenter {
public:
	ImageSegmenter(string path, float sigma);
	ImageSegmenter(CImg<unsigned char>& input);
	// Tools methods
	void performKmeans(CImg<unsigned char>& input);
	bool isEqual(position& a, position& b);
	position getMeans(vector<position>& set, CImg<unsigned char>& src);
	static CImg<unsigned char> toGrayScale(CImg<unsigned char>& input);
	// Getter
	CImg<unsigned char> generate(CImg<unsigned char>& input);
	CImg<unsigned char> getOutputImg();
	CImg<unsigned char> getBlockImg();

private:
	// Basic CImg store
	CImg<unsigned char> result;
	CImg<unsigned char> block;
	CImg<unsigned char> grayImg;
	// K-Means state variables
	position core1, core2;
	stack<HoughPos> posStack;
	vector<vector<bool> > visited;

};

#endif