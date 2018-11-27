#include "Matching.h"
#include "Blending.h"

int main(int argc, char* argv[]){
	// Get input images addresses
	if(argc < 3){
		cout << "Not enough parameters!" << endl;
		return -1;
	}
	// SIFT steps
	CImg<float> src1, src2;
	src1.load_bmp(argv[1]);  src2.load_bmp(argv[2]);
	cout << "Begin SIFT" << endl;
	SIFT sifter1(src1);
	sifter1.SIFT_Start();
	sifter1.saveKeyPointsImg("./output/kp1.bmp");

	SIFT sifter2(src2);
	sifter2.SIFT_Start();
	sifter2.saveKeyPointsImg("./output/kp2.bmp");
	cout << "End SIFT" << endl;

	Matcher matcher(sifter1.GetFirstKeyDescriptor(), sifter2.GetFirstKeyDescriptor());
	matcher.matchProcess();
	matcher.drawOriKeyPoints(argv[1], argv[2], "./output/kp1_real.bmp", "./output/kp2_real.bmp");
	matcher.mixImageAndDrawLine("./output/mixImg.bmp", "./output/mixImgWithLine.bmp");
	matcher.RANSAC("./output/fixed_mixLinedImg.bmp");

	Blender blender(matcher.getMatchVec().x, matcher.getMatchVec().y);
	blender.BlendImages(argv[1], argv[2], "./output/blendedImg.bmp");
	return 0;
}