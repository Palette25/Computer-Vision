/*
*  Program: Matching.h
*  Usage: Define align and match image methods -- RANSAC
*/
#ifndef MATCHING_H
#define MATCHING_H

#include "CImg.h"
#include "SIFT.h"
#include <vector>

#define FeatureDescGap 1.0
#define InliersGap 500.0

using namespace std;
using namespace cimg_library;

struct Point{
	int x, y;
	Point(){
		x = y = -1;
	}
	Point(int x_, int y_){
		x = x_;
		y = y_;
	}
};

struct MatchedPair{
	float minDis;
	Point keyPoint1;
	Point keyPoint2;
	MatchedPair(Point p1, Point p2, float dis){
		keyPoint1 = p1;
		keyPoint2 = p2;
		minDis = dis;
	}
};

class Matcher{
public:
	Matcher(){}
	Matcher(vector<KeyPoint> kp1, vector<KeyPoint> kp2);
	// Match and align process
	void matchProcess();
	void RANSAC(char* fileName);

	// Draw results function
	void drawOriKeyPoints(char* fileNameA, char* fileNameB, char* saveAddrA, char* saveAddrB);
	void drawRealKeyPoints(char* fileName, int maxIndex);
	void mixImageAndDrawLine(char* mixImg, char* lineImg);
	Point getMatchVec();

private:
	CImg<float> srcImg1, srcImg2;
	CImg<float> srcImgWithKp1, srcImgWithKp2;
	CImg<float> mixImg;
	CImg<float> result;
	// Key points count
	int count1, count2;
	vector<KeyPoint> keyDescriptor1;
	vector<KeyPoint> keyDescriptor2;
	// Matched pairs
	vector<MatchedPair> pairSet;
	Point matchVec;

};

#endif