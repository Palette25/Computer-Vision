/*
* Program: Blending.h
* Usage: Blend all the images
*/
#ifndef BLENDING_H
#define BLENDING_H

#include "CImg.h"
#include <iostream>

using namespace std;
using namespace cimg_library;

struct Vector{
	int x, y;
	Vector(){}
	Vector(int x_, int y_){
		x = x_;
		y = y_;
	}
};


class Blender{
public:
	Blender(int x, int y);
	void BlendImages(char* fileNameA, char* fileNameB, char* saveAddr);

private:
	Vector matchVec;  // Blending vector
	CImg<float> srcImg1, srcImg2;
	CImg<float> result;
};

#endif