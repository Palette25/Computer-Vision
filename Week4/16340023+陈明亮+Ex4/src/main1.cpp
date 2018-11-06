/*
* Main1.cpp - For Color Image Transform
*/
#include "Color_Transform.h"

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << "Please enter source color image path and source background image path!" << endl;
		exit(1);
	}
	CImg<float> src1(argv[1]);
	CImg<float> src2(argv[2]);

	Color_Transformer cter(src1, src2);
	cter.Color_Transform();

	return 0;
}