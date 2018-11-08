#include "CImg.h"

using namespace cimg_library;

int main(){
	CImg<float> test("../frameImg/dest.bmp");
	test.resize(323, 400);
	test.display();
	return 0;
}