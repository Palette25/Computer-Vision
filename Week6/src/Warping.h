/*
*  Program: Warping.h
*  Usage: Define image warping methods
*/
#ifndef WARPING_H_
#define WARPING_H_

#include <iostream>
#include "Utils.h"

using namespace std;
using namespace cimg_library;

struct Axis{
	float p1, p2, p3, p4;
	float p5, p6, p7, p8;
	Axis(){}
	Axis(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8){
		p1 = a1;  p2 = a2;  p3 = a3;  p4 = a4;
		p5 = a5;  p6 = a6;  p7 = a7;  p8 = a8;
	}
	void print(){
		cout << p1 << " " << p2 << " " << p3 << " " << p4 << endl;
		cout << p5 << " " << p6 << " " << p7 << " " << p8 << endl;
	}
};

class Warper{
public:
	Warper();
	// Start-up of warping, using homography
	void warpImage(CImg<unsigned char> &src, CImg<unsigned char> &dst, Axis ax, int offset_x, int offset_y);
	// Warping tools functions
	float getMaxXAfterWarping(CImg<unsigned char> &input, Axis ax_);
	float getMinXAfterWarping(CImg<unsigned char> &input, Axis ax_);
	float getMaxYAfterWarping(CImg<unsigned char> &input, Axis ax_);
	float getMinYAfterWarping(CImg<unsigned char> &input, Axis ax_);
	int getHeightAfterWarping(CImg<unsigned char> &input, Axis ax_);
	int getWidthAfterWarping(CImg<unsigned char> &input, Axis ax_);
	float getXAfterWarping(float x, float y, Axis a){
		return a.p1 * x + a.p2 * y + a.p3 * x * y + a.p4;
	}
	float getYAfterWarping(float x, float y, Axis a){
		return a.p5 * x + a.p6 * y + a.p7 * x * y + a.p8;
	}
	// Must move images to fix best after warping
	void moveImageByOffset(CImg<unsigned char> &src, CImg<unsigned char> &dst, float offset_x, float offset_y);

private:
	CImg<unsigned char> srcImg;
	Utils ut;
	Axis ax;

};

#endif