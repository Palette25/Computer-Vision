/*
* Program: Matching.cpp
* Usage: Implement RANSAC, Align neighbor pairs
*/
#include "Matching.h"

Matcher::Matcher(vector<KeyPoint> kp1, vector<KeyPoint> kp2){
	this->keyDescriptor1 = kp1;
	this->keyDescriptor2 = kp2;
	this->count1 = kp1.size();
	this->count2 = kp2.size();
}

// Match and align process, start-up
void Matcher::matchProcess(){
	for(KeyPoint temp_point1 : keyDescriptor1){
		float x = temp_point1.x,
			  y = temp_point1.y;

		float minSSD = 100.0;
		int minIndex = -1, x1 = -1, y1 = -1;
		for(KeyPoint temp_point2 : keyDescriptor2){
			float ssd = 0;
			float* desc1 = temp_point1.descriptors;
			float* desc2 = temp_point2.descriptors;
			for(int i=0; i<128; i++){
				ssd += desc1[i] + desc2[i];
			}
			if(ssd < minSSD){
				minSSD = ssd;
				x1 = temp_point2.x;
				y1 = temp_point2.y;
			}
		}
		if(minSSD < FeatureDescGap){
			Point p1(temp_point1.x, temp_point1.y);
			Point p2(x1, y1);
			this->pairSet.push_back(MatchedPair(p1, p2, minSSD));
		}
	}
}

void Matcher::RANSAC(char* fileName){
	int maxInlierNum = 0, maxIndex = -1, inlinerNum = 0;
	// Match every pair of matched points
	for(int i=0; i<pairSet.size(); i++){
		inlinerNum = 0;
		int x0 = pairSet[i].keyPoint1.x,
			y0 = pairSet[i].keyPoint1.y;
		int x1 = pairSet[i].keyPoint2.x + this->srcImg1.width(),
			y1 = pairSet[i].keyPoint2.y;

		int deltaX = x1 - x0, deltaY = y1 - y0;
		// Find inliner
		for(int j=0; j<pairSet.size(); j++){
			if(j != i){
				int jx0 = pairSet[j].keyPoint1.x,
					jy0 = pairSet[j].keyPoint1.y;
				int jx1 = pairSet[j].keyPoint2.x + this->srcImg1.width(),
					jy1 = pairSet[j].keyPoint2.y;

				int jdeltaX = jx1 - jx0;
				int jdeltaY = jy1 - jy0;
				int vecGap = (jdeltaX - deltaX) * (jdeltaX - deltaX) + (jdeltaY - deltaY) * (jdeltaY - deltaY);

				if(vecGap < InliersGap){
					inlinerNum++;
				}
			}
		}
		if(inlinerNum > maxInlierNum){
			maxInlierNum = inlinerNum;
			maxIndex = i;
		}
	}
	drawRealKeyPoints(fileName, maxIndex);
}

// Draw results function
void Matcher::drawOriKeyPoints(char* fileNameA, char* fileNameB, char* saveAddrA, char* saveAddrB){
	this->srcImg1.load_bmp(fileNameA);
	this->srcImgWithKp1 = CImg<float>(srcImg1.width(), srcImg1.height(), 1, 3, 1);
	cimg_forXY(srcImgWithKp1, x, y){
		srcImgWithKp1(x, y, 0, 0) = srcImg1(x, y, 0, 0);
		srcImgWithKp1(x, y, 0, 1) = srcImg1(x, y, 0, 1);
		srcImgWithKp1(x, y, 0, 2) = srcImg1(x, y, 0, 2);
	}
	this->srcImg2.load_bmp(fileNameB);
	this->srcImgWithKp2 = CImg<float>(srcImg2.width(), srcImg2.height(), 1, 3, 1);
	cimg_forXY(srcImgWithKp2, x, y){
		srcImgWithKp2(x, y, 0, 0) = srcImg2(x, y, 0, 0);
		srcImgWithKp2(x, y, 0, 1) = srcImg2(x, y, 0, 1);
		srcImgWithKp2(x, y, 0, 2) = srcImg2(x, y, 0, 2);
	}
	const double yellow[] = {255, 255, 0};
	for(int i=0; i<pairSet.size(); i++){
		srcImgWithKp1.draw_circle(pairSet[i].keyPoint1.x, pairSet[i].keyPoint1.y, 3, yellow, 1.0f);
		srcImgWithKp2.draw_circle(pairSet[i].keyPoint2.x, pairSet[i].keyPoint2.y, 3, yellow, 1.0f);
	}
	srcImgWithKp1.display("srcImgWithKp1");
	srcImgWithKp1.save(saveAddrA);
	srcImgWithKp2.display("srcImgWithKp2");
	srcImgWithKp2.save(saveAddrB);
}

void Matcher::drawRealKeyPoints(char* fileName, int maxIndex){
	result = CImg<float>(this->srcImg1.width() + this->srcImg2.width(), srcImg1.height(), 1, 3, 0);
	// Fill the result matched image
	cimg_forXY(result, x, y){
		if(x < srcImg1.width()){
			if(y < srcImg1.height()){
				result(x, y, 0, 0) = srcImgWithKp1(x, y, 0, 0);
				result(x, y, 0, 1) = srcImgWithKp1(x, y, 0, 1);
				result(x, y, 0, 2) = srcImgWithKp1(x, y, 0, 2);	
			}else {
				// Else fill with black = {0, 0, 0}
				result(x, y, 0, 0) = 0;
				result(x, y, 0, 1) = 0;
				result(x, y, 0, 2) = 0;
			}
		}else {
			if(y < srcImg2.height()){
				result(x, y, 0, 0) = srcImgWithKp2(x - srcImg1.width(), 0, 0);
				result(x, y, 0, 1) = srcImgWithKp2(x - srcImg1.width(), 0, 1);
				result(x, y, 0, 2) = srcImgWithKp2(x - srcImg1.width(), 0, 2);
			}else {
				// Else fill with black = {0, 0, 0}
				result(x, y, 0, 0) = 0;
				result(x, y, 0, 1) = 0;
				result(x, y, 0, 2) = 0;
			}
		}
	}

	int max_x0 = pairSet[maxIndex].keyPoint1.x,
		max_y0 = pairSet[maxIndex].keyPoint1.y;
	int max_x1 = pairSet[maxIndex].keyPoint2.x,
		max_y1 = pairSet[maxIndex].keyPoint2.y;
	int deltaX = max_x1 - max_x0, deltaY = max_y1 - max_y0;
	matchVec = Point(deltaX, deltaY);

	const double color[] = {0, 255, 255};
	for(int j=0; j<pairSet.size(); j++){
		int x0 = pairSet[j].keyPoint1.x,
			y0 = pairSet[j].keyPoint1.y;
		int x1 = pairSet[j].keyPoint2.x,
			y1 = pairSet[j].keyPoint2.y;
		int tempDeltaX = x1 - x0, tempDeltaY = y1 - y0,
			vectorGap = (tempDeltaX - deltaX) * (tempDeltaX - deltaX) + (tempDeltaY - deltaY) * (tempDeltaY - deltaY);
		
		if(vectorGap < InliersGap){
			result.draw_line(x0, y0, x1, y1, color);
		}
	}
	result.display("Fixed_Line_Image");
	result.save(fileName);
}

void Matcher::mixImageAndDrawLine(char* mixImgAddr, char* lineImgAddr){
	this->mixImg = CImg<float>(srcImg1.width() + srcImg2.width(), max(srcImg1.height(), srcImg2.height()), 1, 3, 0);
	cimg_forXY(mixImg, x, y){
		if(x < srcImg1.width()){
			if(y < srcImg1.height()){
				mixImg(x, y, 0, 0) = srcImgWithKp1(x, y, 0, 0);
				mixImg(x, y, 0, 1) = srcImgWithKp1(x, y, 0, 1);
				mixImg(x, y, 0, 2) = srcImgWithKp1(x, y, 0, 2);
			}else {
				// Else fill with black point
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}else {
			if(y < srcImg2.height()){
				mixImg(x, y, 0, 0) = srcImgWithKp2(x - srcImg1.width(), y, 0 ,0);
				mixImg(x, y, 0, 1) = srcImgWithKp2(x - srcImg1.width(), y, 0 ,1);
				mixImg(x, y, 0, 2) = srcImgWithKp2(x - srcImg1.width(), y, 0 ,2);
			}else {
				// Else fill with black point
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}
	}
	mixImg.display("MixImage");
	mixImg.save(mixImgAddr);

	float blue[] = {0, 255, 255};
	for(int i=0; i<pairSet.size(); i++){
		int x0 = pairSet[i].keyPoint1.x,
			y0 = pairSet[i].keyPoint1.y;
		int x1 = pairSet[i].keyPoint2.x + srcImg1.width(),
			y1 = pairSet[i].keyPoint2.y;
		mixImg.draw_line(x0, y0, x1, y1, blue);
	}
	mixImg.display("MixLineImage");
	mixImg.save(lineImgAddr);
}

Point Matcher::getMatchVec(){
	return this->matchVec;
}