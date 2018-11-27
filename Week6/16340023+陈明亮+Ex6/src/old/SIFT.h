/*
*  Program: SIFT.h
*  Usage: Define C++ class to deal and realize SIFT process
*/
#ifndef SIFT_H
#define SIFT_H

#include "CImg.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace cimg_library;

#define GAUSSKERN 3.5
#define PI 3.14159265358979323846
#define INITSIGMA 0.5
#define SCALESPEROCTAVE 2
#define CONTRAST_THRESHOLD  0.02
#define CURVATURE_THRESHOLD 10.0
#define MAXOCTAVES 4

// Structure of every level in octave
struct ImageLevel{
	int length;
	float sigma;
	float absolute_sigma;
	CImg<float> level;
};
// Structure of every octave
struct ImageOctave{
	int width, height;
	float subSample;
	ImageLevel octave[5];
};
// Structure of a key point in an image
struct KeyPoint{
	int height, width;  // Origin image's height and width
	float x = -1, y = -1;  // Key point's location
	int octave_order, level_order;  // Key point in which octave and level
	// Points self-private variables
	float scale, ori, mag;  // The point's level's scale, orientation, and magniture
	float* descriptors;  // The key description word
};

class SIFT{
public:
	SIFT(CImg<float> src);
	// SIFT process steps
	// 1. Preprocess input image
	CImg<float> InitImage(CImg<float> input);
	// 2. Establish gaussian octaves
	ImageOctave* BuildGaussianOctave(CImg<float> input);
	// 3. Detect key points locations
	int DetectKeyPoints();
	void DisplayKeyPoints();
	// 4. Calculate gaussian image's gradients, magniture and direction
	void ComputeGrad_Mag_Dir();
	// Nearest neighbor binlinear filtering
	int FindNearestRotationNei(float angle, int count);
	void AverageWeakBins(float* bins, int count);

	bool InterpolateOrientation(float left, float middle, float right, float* correction, float* peak);
	void AssignTheMainOrientation();
	void DisplayOrientation(CImg<float> img);
	// 5. Extract key points' feature descriptors
	void ExtractFeatureDescriptors();

	// SIFT matrix tools functions
	CImg<float> makeHalfSize(CImg<float> input);  // Using down sampling
	CImg<float> makeDoubleSize(CImg<float> input);  // Using up sampling
	CImg<float> makeDoubleSizeLinear(CImg<float> input);  // Using up smapling and linear interpolation
	CImg<float> make2DGaussianKernel(float sigma);  // Get 2D gaussian kernel

	float getPixelBi(CImg<float> input, int row, int col);  // Bilinear interpolation
	float getNormVector(float input[], int dim);
	// Pyrmids associate functions
	CImg<float> ImgJoinHorizen(CImg<float> img1, CImg<float> img2);
	CImg<float> ImgJoinVertical(CImg<float> img1, CImg<float> img2);
	// Gray tool function
	CImg<float> toGrayImage(CImg<float> img);
	CImg<float> convertScale(CImg<float> src, float scale, float shift);

	// Main process startup functions
	void SIFT_Start();
	vector<KeyPoint> GetFirstKeyDescriptor();
	void saveKeyPointsImg(char* fileName);


private:
	CImg<float> srcImg;
	CImg<float> keyPointsImg;
	// Octave infos of input image
	int octavesNum;
	vector<ImageOctave> octaves;  // store all octaves
	vector<ImageOctave> DOGoctaves;  // Here choose DOG cause it's easy to write
	vector<ImageOctave> magPyr;  // Store Magniture octaves
	vector<ImageOctave> gradPyr;  // Store gradient octaves
	// Feature key points
	int keyPointsNum = 0;
	vector<KeyPoint> keyPoint;  // Store all the keyPoints
	vector<KeyPoint> keyDescriptors;  // Store all the descriptors of keypoints

};

#endif