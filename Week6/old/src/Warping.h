/*
*  Program: Warping.h
*  Usage: Define image warping methods
*/
#ifndef WARPING_H_
#define WARPING_H_

#include "Matching.h"
#include <iostream>

using namespace std;

class Warper{
public:
	Warper();
	// Start-up of warping, using homography
	CImg<float> warpImage(CImg<float> &src, CImg<float> &dst, Axis ax, int offset_x, int offset_y);
	// Warping tools functions
	float getMaxXAfterWarping(CImg<float> &input, Axis ax_);
	float getMinXAfterWarping(CImg<float> &input, Axis ax_);
	float getMaxYAfterWarping(CImg<float> &input, Axis ax_);
	float getMinYAfterWarping(CImg<float> &input, Axis ax_);
	int getHeightAfterWarping(CImg<float> &input, Axis ax_);
	int getWidthAfterWarping(CImg<float> &input, Axis ax_);
	float getXAfterWarping(float x, float y, Axis a){
		return a.p1 * x + a.p2 * y + a.p3 * x * y + a.p4;
	}
	float getYAfterWarping(float x, float y, Axis a){
		return a.p5 * x + a.p6 * y + a.p7 * x * y + a.p8;
	}
	// Must move images to fix best after warping
	CImg<float> moveImageByOffset(CImg<float> &src, CImg<float> &dst, float offset_x, float offset_y);

private:
	CImg<float> srcImg;
	Axis ax;

};

#endif