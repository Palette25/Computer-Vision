#ifndef REGIONGROWING_H
#define REGIONGROWING_H

#include "CImg.h"
#include <iostream>
#include <stack>

#define HIGHTHRESHOLD 255

using namespace cimg_library;
using namespace std;

struct GPoint{
	int x, y;
	GPoint(){}
	GPoint(int x_, int y_){
		this->x = x_;
		this->y = y_;
	}
	// Operators
	GPoint operator+(const GPoint& a) const{
		return GPoint(x+a.x, y+a.y);
	}
	GPoint operator-(const GPoint& a) const{
		return GPoint(x-a.x, y-a.y);
	}
};

class RegionGrower{
public:
	RegionGrower(CImg<float>& input, int low);
	CImg<float> toGrayScale();
	CImg<float> edgeDetection(CImg<float>& input);
	CImg<float> growResult();

private:
	CImg<float> srcImg;
	int width, height;
	int seedX, seedY;
	int lowThreshold;

};

#endif