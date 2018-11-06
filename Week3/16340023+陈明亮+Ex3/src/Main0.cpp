/*
*  Main Program of A4 edge detector and coins detector
*/

#include "A4_Edge_detector.h"
#include "CANNY.h"

int main(int argc, char* argv[]){
	if(argc < 3){
		Edge_detector::usage(argv[0]);
		return 1;
	}
	int sigma = atof(argv[2]);
	cimg::exception_mode(0);
	CImg<float> input(argv[1]);
	Edge_detector detector(input,sigma);
	detector.Edge_detection();
	return 0;
}