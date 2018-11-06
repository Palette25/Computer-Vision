/*
*  Program: Image_Equalizater.cpp
*/
#include "Image_Equalizater.h"

/*
* Usage: Construct Method
* Parameters: 
*		1. source image
* 		2. type 0 for gray image input, type 1 for colorful image input
*/
Histogram_Equalizater::Histogram_Equalizater(CImg<unsigned char>& src, bool type){
	this->srcImg = src;
	this->img_type = type;
	if(type == 0){
		this->destImg = CImg<unsigned char>(src.width(), src.height(), src.depth(), 1, 0);
		RGBtoGray();
	}else {
		this->destImg = CImg<unsigned char>(src.width(), src.height(), src.depth(), 3);
	}
}

void Histogram_Equalizater::RGBtoGray(){
	CImg<unsigned char> gray(destImg.width(), destImg.height(), 1, 1, 0);
	cimg_forXY(srcImg, x, y){
		gray(x, y) = srcImg(x, y, 0) * 0.299 + srcImg(x, y, 1) * 0.587 + srcImg(x, y ,2) * 0.114;
	}
	srcImg = gray;
}

/*
* Usage: Calculate histogram for input image
* Parameter: Origin image input
* Output: Histogram of this image
*/
vector<float> Histogram_Equalizater::calculateHistogram(CImg<unsigned char>& origin){
	vector<float> hist(256, 0);
	int total = 0;
	cimg_forXY(origin, x, y){
		++hist[origin(x, y)];
		++total;
	}
	// Perform probability transform
	for(int i=0; i<256; i++){
		hist[i] = (float)hist[i] / total;
	}

	return hist;
}

vector<rgb> Histogram_Equalizater::calculateRGBHistogram(CImg<unsigned char>& origin){
	vector<rgb> hist;
	int total = 0;
	for(int i=0; i<256; i++){
		hist.push_back(rgb(0, 0, 0));
	}
	cimg_forXY(origin, x, y){
		hist[origin(x, y, 0)].r += 1;
		hist[origin(x, y, 1)].g += 1;
		hist[origin(x, y, 2)].b += 1;
		++total;
	}
	// Perform probability dividing
	for(int i=0; i<256; i++){
		hist[i].r = (float)hist[i].r / total;
		hist[i].g = (float)hist[i].g / total;
		hist[i].b = (float)hist[i].b / total;
	}
	return hist;
}

/*
* Usage: Start Equalization
*/
void Histogram_Equalizater::equalization(){
	if(img_type == 0){
		// Perform gray image transform
		srcHistogram = calculateHistogram(srcImg);
	
		// Perform pixels probability adding
		float sum = 0;
		vector<int> thist(256, 0);
		for(int i=0; i<256; i++){
			sum += srcHistogram[i];
			thist[i] = (int)(sum * (256-1) + 0.5f);
		}
		// Perform filling into destination image
		cimg_forXY(destImg, x, y){
			destImg(x, y) = thist[srcImg(x, y)];
		}
	}else {
		// Perform colorful image transform
		vector<rgb> colorhist = calculateRGBHistogram(srcImg);

		float sum1 = 0, sum2 = 0, sum3 = 0;
		vector<rgb> thist;
		for(int i=0; i<256; i++){
			thist.push_back(colorhist[i]);
		}
		// Adding
		for(int i=0; i<256; i++){
			sum1 += thist[i].r;
			sum2 += thist[i].g;
			sum3 += thist[i].b;
			thist[i].r = (sum1 * 255 + 0.5f);
			thist[i].g = (sum2 * 255 + 0.5f);
			thist[i].b = (sum3 * 255 + 0.5f);
		}
		// Finish
		cimg_forXY(destImg, x, y){
			destImg(x, y, 0) = (int)thist[srcImg(x, y, 0)].r;
			destImg(x, y, 1) = (int)thist[srcImg(x, y, 1)].g;
			destImg(x, y, 2) = (int)thist[srcImg(x, y, 2)].b;
		}
	}
	srcImg.display();
	// Print histogram of input and output
	CImg<int> histImg = srcImg.histogram(256, 0, 255);
	histImg.display_graph("Source hist", 3);
	// Display final result
	destImg.display();
	CImg<int> histdImg = destImg.histogram(256, 0, 255);
	histdImg.display_graph("Dest hist", 3);
}