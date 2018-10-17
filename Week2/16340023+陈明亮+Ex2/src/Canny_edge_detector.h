#ifndef CANNY_EDGE_DETECTOR_H
#define CANNY_EDGE_DETECTOR_H

#include "CImg.h"
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

#define abs(x) ( (x) >= 0 ? x : -x)
#define GAP_NUM 0.005f
#define MAG_SCALE 100.0f
#define MAG_LIMIT 1000.0f
#define MAG_MAX (MAG_SCALE * MAG_LIMIT)

using namespace cimg_library;
using namespace std;

class Edge_detector{
public:
	Edge_detector(CImg<unsigned char>& image, float sigma, float tlow, float thigh);
	~Edge_detector();
	// Startup of the edge detection
	CImg<unsigned char>& edge_detection();
	void gaussian_smooth();
	void compute_gradients();
	void no_max_supp(int iX, int mX, int iY, int mY, float* xGradient, float* yGradient);
	void apply_hysteresis();
	void follow_edges(int x, int y, int i);
	void remove_less20_edges();
	static void usage(char* name);

private:
	CImg<unsigned char> srcImg;
	CImg<unsigned char> edge;
	int height, width;
	float sigma, tlow, thigh;
	int *edgeImg, *mag;  // The edge image and magnitude array

	inline float hypo(float x, float y){
		return (float)sqrt(x*x + y*y);
	}
	inline float gauss(float x, float sig){
		return (float)exp(-(x*x) / (2.0 * sig *  sig));
	}
};

#endif