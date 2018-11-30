/*
* Program: Matching.h
* Usage: Match all the images with key points
*/
#ifndef MATCHING_H
#define MATCHING_H

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "CImg.h"

#define NUM_OF_PAIR 4
#define CONFIDENCE 0.99
#define INLINER_RATIO 0.5
#define RANSAC_THRESHOLD 4.0

using namespace std;
using namespace cimg_library;

struct keyPointPair{
	VlSiftKeypoint p1;
	VlSiftKeypoint p2;
	keyPointPair(VlSiftKeypoint _p1, VlSiftKeypoint _p2){
		p1 = _p1;
		p2 = _p2;
	}
};

struct keyPointPairs{
	vector<keyPointPair> pairs;
	int src, dst;
	point_pairs(vector<keyPointPair> _pairs, int s, int d) {
		pairs = _pairs;
		src = s;
		dst = d;
	}
};

struct Axis{
	float p1, p2, p3, p4;
	float p5, p6, p7, p8;
	Axis(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8){
		p1 = a1;  p2 = a2;  p3 = a3;  p4 = a4;
		p5 = a5;  p6 = a6;  p7 = a7;  p8 = a8;
	}
	void print(){
		cout << p1 << " " << p2 << " " << p3 << " " << p4 << endl;
		cout << p5 << " " << p6 << " " << p7 << " " << p8 << endl;
	}
};

class Matcher{
public:
	Macther(){

	}
	vector<keyPointPair> scanPointPairs(map<vector<float>, VlSiftKeypoint> f1, map<vector<float>, VlSiftKeypoint> f2);
	vector<int> getInlinerIndexs(vector<keyPointPair> pairs, Axis H, set<int> indexes);

	Axis Homography(vector<keyPointPair> pairs); 
	Axis RANSAC(vector<keyPointPair> pairs);
	Axis leastSquare(vector<keyPointPair> pairs, vector<int> inliner_indexs);
	inline int numberOfIterations(float p, float w, int num) {
		return ceil(log(1 - p) / log(1 - pow(w, num)));
	}

	inline int random(int min, int max) {
		return rand() % (max - min + 1) + min;
	}

private:
	Warper wr;
};


#endif