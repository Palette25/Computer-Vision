/*
* Program: Color_Transform.cpp
*/
#include "Color_Transform.h"

Color_Transformer::Color_Transformer(CImg<float>& src1, CImg<float>& src2){
	this->colorImg = src1;
	this->srcImg = src2;
	this->destImg = CImg<float>(src2.width(), src2.height(), 1, 3, 0);
}
// Color channels transform tool functions
void Color_Transformer::RGBtoLab(){
	colorImg = colorImg.get_RGBtoLab();
	srcImg = srcImg.get_RGBtoLab();
}

void Color_Transformer::LabToRGB(){
	destImg = destImg.get_LabtoRGB();
}

// Start-up
void Color_Transformer::Color_Transform(){
	RGBtoLab();

	// Calculate Lab's means and variances of three channels
	CImg<float> color_l = colorImg.get_channel(0);
	CImg<float> color_a = colorImg.get_channel(1);
	CImg<float> color_b = colorImg.get_channel(2);
	CImg<float> src_l = srcImg.get_channel(0);
	CImg<float> src_a = srcImg.get_channel(1);
	CImg<float> src_b = srcImg.get_channel(2);
	// Getting mean and variance
	float c1_mean = color_l.mean();
	float c2_mean = color_a.mean();
	float c3_mean = color_b.mean();
	float s1_mean = src_l.mean();
	float s2_mean = src_a.mean();
	float s3_mean = src_b.mean();

	float c1_variance = color_l.variance(0);
	float c2_variance = color_a.variance(0);
	float c3_variance = color_b.variance(0);
	float s1_variance = src_l.variance(0);
	float s2_variance = src_a.variance(0);
	float s3_variance = src_b.variance(0);

	cimg_forXY(destImg, x, y){
		destImg(x, y, 0) = (sqrtf(c1_variance) / sqrtf(s1_variance)) * (src_l(x, y) - s1_mean) + c1_mean;
		destImg(x, y, 1) = (sqrtf(c2_variance) / sqrtf(s2_variance)) * (src_a(x, y) - s2_mean) + c2_mean;
		destImg(x, y, 2) = (sqrtf(c3_variance) / sqrtf(s3_variance)) * (src_b(x, y) - s3_mean) + c3_mean;
	}
	// Perform Lab to RGB
	LabToRGB();
	// Display result
	destImg.display();
}