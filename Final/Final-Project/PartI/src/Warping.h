/*
*  Warping.h -- For A4 Paper Warping to suitable location
*/
#ifndef WARPING_H
#define WARPING_H

#include "utils.h"
#include "Projection.h"

class Warper {
public:
	Warper(vector<HoughPos>&, CImg<unsigned char>&, int);
	CImg<unsigned char> getResult(){
		return this->result;
	}

	double srcPos[4][2];
	
private:
	CImg<unsigned char> result;
	// Inline calculating methods
	inline double getXAfterWarping(double, double, Homography &);
    inline double getYAfterWarping(double, double, Homography &);
    inline void warpingImageByHomography(const CImg<unsigned char> &, CImg<unsigned char> &, Homography &);
    inline Homography getHomography(vector<HoughPos>&);
    inline Homography getHomography2(vector<HoughPos>&);

};

#endif