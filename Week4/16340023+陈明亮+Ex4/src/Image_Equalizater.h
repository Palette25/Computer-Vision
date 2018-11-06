/*
*  Program Name: 
*		Image_Equalizater.h
*  Class Name:
*		 Histogram_Equalizater
*  Input: Source image to process
*  Output: Processed image after histogram equalization
*  Usage: 
*		./Image_Equalizater srcImgPath
*/
#ifndef IMAGE_EQUALIZATER_H
#define IMAGE_EQUALIZATER_H

#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

struct rgb{
	float r;
	float g;
	float b;
	rgb(float r_, float g_, float b_){
		r = r_;
		g = g_;
		b = b_;
	}
	rgb& operator=(const rgb& oth){
		r = oth.r;
		g = oth.g;
		b = oth.b;
	}
};

class Histogram_Equalizater{
public:
	Histogram_Equalizater(CImg<unsigned char>& src, bool type);
	vector<float> calculateHistogram(CImg<unsigned char>& origin);
	vector<rgb> calculateRGBHistogram(CImg<unsigned char>& origin);
	void RGBtoGray();
	void equalization();

private:
	int img_type;
	CImg<unsigned char> srcImg;  // Source image
	CImg<unsigned char> destImg;  // Processed image
	vector<float> srcHistogram;  // Source image's color histogram

};

#endif