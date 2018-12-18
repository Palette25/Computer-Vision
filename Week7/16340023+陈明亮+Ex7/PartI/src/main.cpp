#include "RegionGrowing.h"
#include "A4_Correcting.h"

int main(int argc, char* argv[]){
	char* path = new char(20);

	if(argc < 3){
		cout << "Please enter the image path, Region Growing InitValue: ";
		cin >> path;
	}else {
		path = argv[1];
	}

	CImg<float> src(path);

	RegionGrower rg(src, atoi(argv[2]));

	CImg<float> re = rg.growResult();

	Image_Transformer it(re, src);
	CImg<float> result = it.ImageTransform();

	// Save result
	result.save("../result/Correct_Result.bmp");

	delete path;

	return 0;
}