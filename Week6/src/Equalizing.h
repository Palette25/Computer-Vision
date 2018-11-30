/*
* Program: Equalizing.h
* Usage: Equalize final result image, to make light strength equal
*/
#ifndef EQUALIZING_H_
#define EQUALIZING_H_

#include <iostream>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Equalizater{
public:
	Equalizater(CImg<unsigned char>& src);
	CImg<unsigned char> Equalization();

private:
	CImg<unsigned char> srcImg;
	CImg<unsigned char> dstImg;
	vector<int> hist;

};

#endif