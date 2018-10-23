/*
*  HeadFile for image edge detector (A4 paper)
*  Command Line Usage:
*  		$  ./A4_Edge_detector image_path sigma
*  Arguments:
*  		image_path -- Path of target image
*		sigma -- Gaussian Smooth's sigma
*  Output:
		1. Edge image with blue line -- I_edge
		2. A4 paper lines equations 
		3. A4 paper four edge points in read circle
*/
#ifndef A4_EDGE_DETECTOR_H
#define A4_EDGE_DETECTOR_H

#include <iostream>
#include <vector>
#include <cmath>
#include "CImg.h"

#define GRADIENT_BOUND 30
#define MIN_PEAK_DISTANCE 60
#define VOTE_THRESHOLD 1000

using namespace cimg_library;
using namespace std;

struct line{
	float k, b;  // y = k*x + b
	line(float k_, float b_) : k(k_), b(b_){}
};

struct point{
	int x, y, count;
	point(int x_, int y_, int count_){
		x = x_;
		y = y_;
		count = count_;
	}
};

class Edge_detector {
public:
	Edge_detector(CImg<float> in, float sigma);
	// Image Process methods
	void RGB_to_gray();  // Turn RGB image to gray scale image
	void Print_lines_equations(int order, double k, double b);  // Print lines equations to terminal
	void Init_hough_space();  // Init hough space
	void Edge_detection();  // Process with hough algorithm
	void find_peaks();  // Find peaks with hough algorithm
	CImg<unsigned char> draw_lines(double color[3]);  // Produce line image with input color
	CImg<unsigned char> draw_points(double color[3], int radius); // Produce points image with input color
	static void usage(char* progName);

private:
	CImg<float> input;  // Input image
	CImg<float> hough_space;  // Hough space after initializing
	CImg<float> gaussian_img; // img after gaussian blur
	vector<point> peaks;  // Peaks after voting in hough algorithm
	vector<point> intersections;  // Intersection points of lines
	vector<line> lines;
	float sigma;
	int width, height;
};

#endif