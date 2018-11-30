/*
* Program: Matching.h
* Usage: Match all the images with key points
*/
#ifndef MATCHING_H
#define MATCHING_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include "CImg.h"
#include "Warping.h"

extern "C"{
	#include <vl/generic.h>
	#include <vl/sift.h>
	#include <vl/kdtree.h>
}

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

class Matcher{
public:
	Macther(){
		this->wr = Warper();
	}
	vector<keyPointPair> scanPointPairs(map<vector<float>, VlSiftKeypoint>& f1, map<vector<float>, VlSiftKeypoint> f2);
	vector<int> getInlinerIndexs(vector<keyPointPair>& pairs, Axis H, set<int> indexes);

	Axis Homography(vector<keyPointPair>& pairs); 
	Axis RANSAC(vector<keyPointPair>& pairs);
	Axis leastSquares(vector<keyPointPair> pairs, vector<int> inliner_indexs);
	inline int random(int min, int max) {
		return rand() % (max - min + 1) + min;
	}

private:
	Warper wr;
};


#endif