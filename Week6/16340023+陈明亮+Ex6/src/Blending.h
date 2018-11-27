/*
* Program: Blending.h
* Usage: Blend result images into one stitched image
*/
#ifndef BLENDING_H
#define BLENDING_H

#include <iostream>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Blender{
public:
	Blender(CImg<float> src1, CImg<float> src2);
	CImg<float> blendImages();

private:
	CImg<float> srcImg1;
	Cimg<float> srcImg2;
	// Judge empty function
	inline bool empty(CImg<float> img, int x, int y){
		if(img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0)
			return true;
		else
			return false;
	}
};

#endif