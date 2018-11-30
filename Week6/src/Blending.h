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
	Blender(CImg<unsigned char>& src1, CImg<unsigned char>& src2);
	CImg<unsigned char> blendImages();

private:
	CImg<unsigned char> srcImg1;
	CImg<unsigned char> srcImg2;
	// Judge empty function
	inline bool empty(CImg<unsigned char> img, int x, int y){
		if(img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0)
			return true;
		else
			return false;
	}
};

#endif