/*
*  Canny.h -- Detect A4 Paper's edges
*/
#ifndef CANNY_H
#define CANNY_H

#include "utils.h"

CImg<unsigned char> EdgeDetection(CImg<unsigned char> input, int length);

#endif