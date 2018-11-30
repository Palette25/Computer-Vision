/*
* Program: Blending.cpp
* Usage: Implements all blending methods
*/
#include "Blending.h"

Blender::Blender(int x, int y){
	this->matchVec.x = x;
	this->matchVec.y = y;
}

void Blender::BlendImages(char* fileNameA, char* fileNameB, char* saveAddr){
	// Init images in blender
	this->srcImg1.load_bmp(fileNameA);
	this->srcImg2.load_bmp(fileNameB);
	this->result = CImg<float>(srcImg1.width() + srcImg2.width() - matchVec.x, 
								srcImg1.height() + abs(matchVec.y), 1, 3, 0);

	// Fill result blending image
	cimg_forXY(result, x, y) {
		if (matchVec.y <= 0) {    //右侧图片需要往下左移动
			if (x < srcImg1.width() && y < srcImg1.height()) {
				if (x >= (srcImg1.width() - matchVec.x) && y >= (0 - matchVec.y)) {    //混合
					result(x, y, 0, 0) = (float)srcImg1(x, y, 0, 0)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 0)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
					result(x, y, 0, 1) = (float)srcImg1(x, y, 0, 1)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 1)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
					result(x, y, 0, 2) = (float)srcImg1(x, y, 0, 2)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 2)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
				}
				else {    //A独在部分
					result(x, y, 0, 0) = srcImg1(x, y, 0, 0);
					result(x, y, 0, 1) = srcImg1(x, y, 0, 1);
					result(x, y, 0, 2) = srcImg1(x, y, 0, 2);
				}
			}
			else if (x >= (srcImg1.width() - matchVec.x) 
				&& y >= (0 - matchVec.y) && y < (0 - matchVec.y) + srcImg2.height()) {    //B独在部分
				result(x, y, 0, 0) = srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 0);
				result(x, y, 0, 1) = srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 1);
				result(x, y, 0, 2) = srcImg2(x - (srcImg1.width() - matchVec.x), y - (0 - matchVec.y), 0, 2);
			}
			else {    //黑色部分
				result(x, y, 0, 0) = 0;
				result(x, y, 0, 1) = 0;
				result(x, y, 0, 2) = 0;
			}
		}
		else {    //matchVec.y > 0; 右侧图片需要往上左移动
			if (x < srcImg1.width() && y >= matchVec.y) {
				if (x >= (srcImg1.width() - matchVec.x) && y < srcImg2.height()) {    //混合
					result(x, y, 0, 0) = (float)srcImg1(x, y - matchVec.y, 0, 0)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 0)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
					result(x, y, 0, 1) = (float)srcImg1(x, y - matchVec.y, 0, 1)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 1)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
					result(x, y, 0, 2) = (float)srcImg1(x, y - matchVec.y, 0, 2)
						* (float)(srcImg1.width() - x) / (float)abs(matchVec.x)
						+ (float)srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 2)
						* (float)(x - (srcImg1.width() - matchVec.x)) / (float)abs(matchVec.x);
				}
				else {    //A独在部分
					result(x, y, 0, 0) = srcImg1(x, y - matchVec.y, 0, 0);
					result(x, y, 0, 1) = srcImg1(x, y - matchVec.y, 0, 1);
					result(x, y, 0, 2) = srcImg1(x, y - matchVec.y, 0, 2);
				}
			}
			else if (x >= (srcImg1.width() - matchVec.x) && y < srcImg2.height()) {    //B独在部分
				result(x, y, 0, 0) = srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 0);
				result(x, y, 0, 1) = srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 1);
				result(x, y, 0, 2) = srcImg2(x - (srcImg1.width() - matchVec.x), y, 0, 2);
			}
			else {    //黑色部分
				result(x, y, 0, 0) = 0;
				result(x, y, 0, 1) = 0;
				result(x, y, 0, 2) = 0;
			}
		}
	}
	result.display("BlendingImg");
	result.save(saveAddr);
}
