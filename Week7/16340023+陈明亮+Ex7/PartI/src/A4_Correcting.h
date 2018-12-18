/*
*  Name: A4_Correcting.h
*  Usage: Define Image Segmentation and Perspective Transform classes
*/
#ifndef A4_CORRECTING_H
#define A4_CORRECTING_H

#include "CImg.h"
#include <iostream>
#include <cmath>
#include <vector>

#define GRADIENT_BOUND 30
#define MIN_PEAK_DISTANCE 60
#define VOTE_THRESHOLD 600
#define STANDARD_WIDTH 1240
#define STANDARD_HEIGHT 1754

using namespace cimg_library;
using namespace std;

struct Line{
	float k, b;
	Line(float k_, float b_){
		this->k = k_;
		this->b = b_;
	}
};

struct Point{
	int x, y, count;
	Point(int x_, int y_, int cnt){
		this->x = x_;
		this->y = y_;
		this->count = cnt;
	}
};

/* Image Transformer class 
* 	1. Use Hough Transform to draw peaks  
*   2. Use Perspective Transform to do modify
*/
class Image_Transformer{
public:
	Image_Transformer(CImg<float>& input, CImg<float>& origin);
	void initHoughSpace();
	void peakDetection();
	void edgeAndPeakDetection();
	// Startip of image transform
	CImg<float> ImageTransform();
	// Do perspective modify
	vector<CImg<float> > getPerspectiveMatrix();
	CImg<float> perspectiveTransform();


private:
	CImg<float> srcImg;
	CImg<float> origin;
	CImg<float> houghSpace;
	// A4 Image edges and peaks
	vector<Line> lines;
	vector<Point> intersactions;
	vector<Point> peaks;

	int width, height;
};

#endif