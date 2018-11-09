/*
* Program: Image_Morphing.h
* Usage: Morph source image to dest image
*/
#ifndef IMAGE_MORPHING_H
#define IMAGE_MORPHING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CImg.h"
#include "Delaunay.h"

using namespace std;
using namespace cimg_library;

struct Point {
	float x, y;
	Point(float a, float b) {
		x = a;
		y = b;
	}
};

struct Triangle {
	Point p1, p2, p3;
	Triangle(Point a, Point b, Point c) : p1(a), p2(b), p3(c) {}
};

struct Three_pair {
	int x, y ,z;
	Three_pair(int x_, int y_, int z_) {
		x = x_;
		y = y_;
		z = z_;
	}
};

class Image_Morpher {
public:
	Image_Morpher(int frameNum);
	void Morphing();
	void initPoints();
	void initTriangles();
	void saveFrames();
	void drawStructedImg();
	bool isInTriangle(Point po, Triangle tr); // Judge whether a point is in the triangle
	CImg<float> getTransformMatrix(Triangle tr1, Triangle tr2); // Get transform matrix

private:
	int frameNum;
	// Images
	CImg<float> srcImg;
	CImg<float> destImg;
	CImgList<float> frameImgList;
	// Face Points of images
	vector<Point> srcPoints;
	vector<Point> destPoints;
	vector<vector<Point> > midFramePoints;
	// Triangles that form images
	vector<Triangle> srcTriangles;
	vector<Triangle> destTriangles;
	vector<vector<Triangle> > midFrameTriangles;
	vector<Delaunay> midFrameMesh;
};

#endif