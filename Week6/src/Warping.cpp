/*
*  Program: Warping.cpp
*  Usage: Implement warping logics
*/
#include "Warping.h"

Warper::Warper(){
	this->ut = Utils();
}

// Start-up of warping, using homography
void Warper::warpImage(CImg<unsigned char> &src, CImg<unsigned char> &dst, Axis ax, int offset_x, int offset_y){
	cimg_forXY(dst, x, y){
		int src_x = getXAfterWarping(x + offset_x, y + offset_y, ax),
			src_y = getYAfterWarping(x + offset_x, y + offset_y, ax);
		if(src_x >= 0 && src_x < src.width() && src_y >= 0 && src_y < src.height()){
			for(int i=0; i<src.spectrum(); i++)
				dst(x, y, i) = ut.bilinearInterpolate(src, src_x, src_y, i);
		}
	}
}

// Warping tools functions
float Warper::getMaxXAfterWarping(CImg<unsigned char> &input, Axis ax_){
	float result = getXAfterWarping(0, 0, ax_);
	if(getXAfterWarping(input.width()-1, 0, ax_) > result){
		result = getXAfterWarping(input.width()-1, 0, ax_);
	}
	if(getXAfterWarping(0, input.height()-1, ax_) > result){
		result = getXAfterWarping(0, input.height()-1, ax_);
	}
	if(getXAfterWarping(input.width()-1, input.height()-1, ax_) > result){
		result = getXAfterWarping(input.width()-1, input.height()-1, ax_);
	}
	return result;
}

float Warper::getMinXAfterWarping(CImg<unsigned char> &input, Axis ax_){
	float result = getXAfterWarping(0, 0, ax_);
	if(getXAfterWarping(input.width()-1, 0, ax_) < result){
		result = getXAfterWarping(input.width()-1, 0, ax_);
	}
	if(getXAfterWarping(0, input.height()-1, ax_) < result){
		result = getXAfterWarping(0, input.height()-1, ax_);
	}
	if(getXAfterWarping(input.width()-1, input.height()-1, ax_) < result){
		result = getXAfterWarping(input.width()-1, input.height()-1, ax_);
	}
	return result;
}

float Warper::getMaxYAfterWarping(CImg<unsigned char> &input, Axis ax_){
	float result = getYAfterWarping(0, 0, ax_);
	if(getYAfterWarping(input.width()-1, 0, ax_) > result){
		result = getYAfterWarping(input.width()-1, 0, ax_);
	}
	if(getYAfterWarping(0, input.height()-1, ax_) > result){
		result = getYAfterWarping(0, input.height()-1, ax_);
	}
	if(getYAfterWarping(input.width()-1, input.height()-1, ax_) > result){
		result = getYAfterWarping(input.width()-1, input.height()-1, ax_);
	}
	return result;
}

float Warper::getMinYAfterWarping(CImg<unsigned char> &input, Axis ax_){
	float result = getYAfterWarping(0, 0, ax_);
	if(getYAfterWarping(input.width()-1, 0, ax_) < result){
		result = getYAfterWarping(input.width()-1, 0, ax_);
	}
	if(getYAfterWarping(0, input.height()-1, ax_) < result){
		result = getYAfterWarping(0, input.height()-1, ax_);
	}
	if(getYAfterWarping(input.width()-1, input.height()-1, ax_) < result){
		result = getYAfterWarping(input.width()-1, input.height()-1, ax_);
	}
	return result;
}

int Warper::getHeightAfterWarping(CImg<unsigned char> &input, Axis ax_){
	int max_height = getMaxYAfterWarping(input, ax_), 
		min_height = getMinYAfterWarping(input, ax_);
	return max_height - min_height;
}

int Warper::getWidthAfterWarping(CImg<unsigned char> &input, Axis ax_){
	int max_width = getMaxXAfterWarping(input, ax_),
		min_width = getMinXAfterWarping(input, ax_);
	return max_width - min_width;
}

// Must move images to fix best after warping
void Warper::moveImageByOffset(CImg<unsigned char> &src, CImg<unsigned char> &dst, float offset_x, float offset_y){
	cimg_forXY(dst, x, y){
		int src_x = x + offset_x, src_y = y + offset_y;
		if(src_x >= 0 && src_x < src.width() && src_y >= 0 && src_y < src.height()){
			for(int i=0; i<src.spectrum(); i++)
				dst(x, y, i) = src(src_x, src_y, i);
		}
	}
}