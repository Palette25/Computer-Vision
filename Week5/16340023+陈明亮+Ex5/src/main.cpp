/*
* Program: Main.cpp
*/
#include "Image_Morphing.h"

int main(int argc, char* argv[]){
	cout << "Image Morphing Begin..." << endl;
	Image_Morpher mor(11);
	mor.Morphing();
	cout << "Image Morphing End..." << endl;
	return 0;
}