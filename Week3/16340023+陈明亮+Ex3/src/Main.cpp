/*
*  Main Program of A4 edge detector and coins detector
*/

#include "Coin_Edge_detector.h"

int main(int argc, char* argv[]){
	if(argc < 6){
		Edge_detector::usage(argv[0]);
		return 1;
	}
	int min_radius = atof(argv[2]), max_radius = atof(argv[3]), votes_threshold = atof(argv[4]);
	int search_threshold = atof(argv[5]);
	cimg::exception_mode(0);
	CImg<float> input(argv[1]);
	Edge_detector detector(input, min_radius, max_radius, votes_threshold, search_threshold);
	detector.Edge_detection();
	return 0;
}