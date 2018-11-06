/*
*  Program Name: 
*		Color_Transform.h
*  Class Name:
*		 Color_Transformer
*  Input: Source image one to provide color, source image two to sccept color
*  Output: Processed image after color transformation
*  Usage: 
*		./Image_Equalizater srcImgOne srcImgTwo
*/
#ifndef COLOR_TRANSFORM_H
#define COLOR_TRANSFORM_H

#include <iostream>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Color_Transformer{
public:
	Color_Transformer(CImg<float>& src1, CImg<float>& src2);
	// Color channels transform tool functions
	void RGBtoLab();
	void LabToRGB();
	// Start-up
	void Color_Transform();


private:
	CImg<float> colorImg;  // The source image provides color
	CImg<float> srcImg;  // The source image provides source background
	CImg<float> destImg;  // Final image after processed
	
};

#endif