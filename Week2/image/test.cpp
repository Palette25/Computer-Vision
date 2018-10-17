#include "CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std;

int main(){
	cimg::exception_mode(0);
	CImg<unsigned char> aa;
	try{
		aa.load_bmp("lena.bmp");
	}catch(CImgException e){
		fprintf(stderr, "CImg Library Error: %s\n", e.what());
	}
	cout << "fuck" << endl;

	int a[2][3] = {{1,1,1}, {2,2,2}};
	(*a)[1] = 5;
	aa(2) = 22;
	cout << aa.height() << endl;
	cout << (aa(33)==aa(33, 0)) << endl;

	return 0;
}