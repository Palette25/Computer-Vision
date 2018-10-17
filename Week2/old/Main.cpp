/*
*  PROGRAM: Main.cpp
*  PURPOSE: The Main.cpp file gets input, instantiate Edge_detector object and invokes Edge_detector's methods
*  to detector all edges and show the result image.
*/

#include "Canny_edge_detector.h"
#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]){
	char* filepath = NULL;  	// Name of the input image file
	char outfilename[128];  	// Name of the output image file
	CImg<unsigned char> image;  // CImg object of the input image
	CImg<unsigned char> edge;   // CImg object of the processed edge image
	float sigma,				// Standard deviation(标准差) of gaussian kernel
		  tlow,                 // Fraction of the high threshold in hysteresis
		  thigh;                // High hysteresisi threshold control

	/********************************
	* Get Command arguments and judge 
	********************************/
	// Argv number less than 5
	if(argc < 5){
		Edge_detector::usage(argv[0]);
		exit(1);
	}

	// Get response parameters
	filepath = argv[1];
	sigma = atof(argv[2]);
	tlow = atof(argv[3]);
	thigh = atof(argv[4]);

	// Read image and judge whether file exist or not bmp type
	cimg::exception_mode(0);
	try{
		image.load_bmp(filepath);
	}catch(CImgException e){
		fprintf(stderr, "CImg Library Error: %s\n", e.what());
		exit(2);
	}
	CImg<unsigned char> gray(image.width(), image.height(), image.depth(), 1);
	cimg_forXY(image, x, y){
		gray(x, y) = image(x, y, 0)*0.299 + image(x, y, 1)*0.587+ image(x, y, 2)*0.114;
	}

	/********************************
	* Realize Edge_detector obect and
	* 	perform edge detection
	********************************/
	Edge_detector canny_detector(gray, sigma, tlow, thigh);
	edge = canny_detector.edge_detection();

	edge.display();

	return 0;
}