/*
*  Partition.h -- Make partition pieces of A4 Paper, to find number's specfic locations
*/
#ifndef PARTITION_H
#define PARTITION_H

#include "utils.h"
#include "Canny.h"
#include "HoughTransform.h"
#include "ImageSegment.h"
#include "Projection.h"

class Partition {
public:
	Partition(CImg<unsigned char>& input, int seq);
	CImg<unsigned char> threshold(CImg<unsigned char>& input, int size, float thres);
	void divideColumn(int col);
	void findDividingLine(int, int);
	vector<int> getColumnLine(CImg<unsigned char>&, int);
	vector<int> getColumnInflectionPoints(vector<int>&);

	// Getter methods
	vector<vector<square> > getSquare();
	CImg<unsigned char> getGrayImg();
	CImg<unsigned char> getDividedImg();

private:
	int seq;
	CImg<unsigned char> grayImg;
	CImg<unsigned char> dividedImg;
	vector<vector<square> > squareVec;
	vector<int> linePos;
	vector<int> blackPixels;


};

#endif