/*
*  Head File for image edge detector (Coins)
*  Command Line Usage:
*		$  ./Coin_Edge_detector image_path sigma
*  Arguments:
*		image_path -- Path of target image
*		sigma -- Gaussian Smooth's sigma
*  Output:
		1. Edge of the circle -- I_edge (in blue color)
		2. Center of each circle (in red color)
		3. Number of the coins
*/
#ifndef COIN_EDGE_DETECTOR_H
#define COIN_EDGE_DETECTOR_H

#include <iostream>
#include <vector>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

class Edge_detector{
public:
	Edge_detector(CImg<unsigned char> input, int min_radius, int max_radius, int votes_threshold, int search_threshold);
	void Edge_detection();
	void RGB_to_gray();
	void Init_hough_space();
	void Hough_circle_transform();
	void draw_circle(int radius);
	static void usage(char* progName);

private:
	CImg<unsigned char> input;
	CImg<unsigned char> gray;
	CImg<unsigned char> gaussian_img;
	CImg<float> hough_space;
	vector<pair<int, int> > votes;  // <int, int> -- votes number, radius
	vector<pair<int, int> > centers;  // <int, int> -- x, y
	vector<pair<int, int> > result;  // Result of all clear circles
	vector<int> final_center_votes;
	int width, height, min_radius, max_radius;
	int circleNumber;
	int votes_threshold, search_threshold;
	int decreaseCount;
	float sigma;
};

#endif