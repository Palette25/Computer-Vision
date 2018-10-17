/*
*  PROGRAM: Hysteresis.h
*  PURPOSE: This head file provides the definitions of Image hysteresis_applyer.
*/

#ifndef HYSTERESIS_H
#define HYSTERESIS_H

#include "CImg.h"

#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0

using namespace cimg_library;

class Hysteresis {
public:
	Hysteresis(CImg<unsigned char>& src, float tlow, float thigh, CImg<float>& angles);
	// Hysteresis methods
	void follow_edges(unsigned char* edgemapptr, short* edgemagptr, short lowval, int cols);
	CImg<unsigned char> apply_hysteresis();
	void non_max_supp();

private:
	float tlow, thigh;
	CImg<unsigned char> srcImg;
	CImg<float> angles;
};

#endif