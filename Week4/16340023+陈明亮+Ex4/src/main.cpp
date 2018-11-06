/*
* Program: Main.cpp
*/
#include <iostream>
#include "Image_Equalizater.h"

using namespace std;

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << "Please enter the source image path and image type!" << endl;
		exit(1);
	}
	int type = atof(argv[2]);
	CImg<unsigned char> src(argv[1]);
	Histogram_Equalizater heter(src, type);
	heter.equalization();

	return 0;
}