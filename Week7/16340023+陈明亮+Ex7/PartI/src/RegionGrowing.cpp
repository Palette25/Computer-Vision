#include "RegionGrowing.h"

const GPoint EightNeighbors[8] = {
	GPoint(1, 0),
	GPoint(1, -1),
	GPoint(0, -1),
	GPoint(-1, -1),
	GPoint(-1, 0),
	GPoint(-1, 1),
	GPoint(0, 1),
	GPoint(1, 1),
};

RegionGrower::RegionGrower(CImg<float>& input, int lowThreshold){
	this->srcImg = input;
	this->width = input.width();
	this->height = input.height();
	this->lowThreshold = lowThreshold;
	this->seedX = input.width() / 2;
	this->seedY = input.height() / 2;
}

CImg<float> RegionGrower::toGrayScale(){
	// Get Gaussian Blur
	this->srcImg = srcImg.get_blur(10.0f);
	CImg<float> grayImg = CImg<float>(srcImg.width(), srcImg.height(), 1, 1, 0);
	cimg_forXY(grayImg, x, y){
		grayImg(x, y, 0) = 0.299 * srcImg(x, y, 0, 0) + 0.587 * srcImg(x, y, 0, 1) 
										+ 0.114 * srcImg(x, y, 0, 2);
	}
	return grayImg;
}

CImg<float> RegionGrower::growResult(){
	CImg<float> gray = toGrayScale();
	gray.display();
	CImg<float> result = CImg<float>(srcImg.width(), srcImg.height(), 1, 1, 0);
	// Start region growing
	stack<GPoint> growQueue;
	growQueue.push(GPoint(seedX, seedY));
	while(!growQueue.empty()){
		GPoint peek = growQueue.top();
		growQueue.pop();
		if(peek.x >= 0 && peek.x <= width-1 && peek.y >= 0 && peek.y <= height-1){
			// Draw this GPoint to white
			result(peek.x, peek.y) = 255;
			// Growing
			for(int i=0; i<8; i++){
				GPoint temp = peek + EightNeighbors[i];
				if(temp.x >= 0 && temp.x <= width-1 && temp.y >= 0 && temp.y <= height-1){
					if(gray(temp.x, temp.y) > lowThreshold && gray(temp.x, temp.y) < HIGHTHRESHOLD && result(temp.x, temp.y) != 255){
						growQueue.push(temp);
					}
				}
			}
		}
	}
	result.display();
	// Get edges by eight neighbors
	result = edgeDetection(result);
	result.display();
	return result;
}

CImg<float> RegionGrower::edgeDetection(CImg<float>& input){
	CImg<float> edge(input.width(), input.height(), 1, 1, 0);
	cimg_forXY(input, x, y){
		GPoint temp(x, y);
		bool blackFlag = false, whiteFlag = false;
		// Start eight neighbors detect
		for(int i=0; i<8; i++){
			GPoint nei = temp + EightNeighbors[i];
			if(nei.x >= 0 && nei.x <= width-1 && nei.y >= 0 && nei.y <= height-1){
				if(input(nei.x, nei.y) == 255){
					whiteFlag = true;
				}else {
					blackFlag = true;
				}
			}
		}
		if(blackFlag && whiteFlag){
			edge(x, y) = 255;
		}
	}

	return edge;
}