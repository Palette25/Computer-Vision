#include "Blending.h"

Blender::Blender(CImg<unsigned char>& src1, CImg<unsigned char>& src2){
	this->srcImg1 = src1;
	this->srcImg2 = src2;
}

CImg<unsigned char> Blender::blendImages(){
	// Construct center og overlapping points
	double sum_x = 0;
	double sum_y = 0;
	int n = 0;

	double sum_x_overlap = 0;
	double sum_y_overlap = 0;
	int overlap_n = 0;
	if (srcImg1.width() > srcImg1.height()) {
		for(int x = 0; x < srcImg1.width(); x++) {
			if(!empty(srcImg1, x, srcImg1.height() / 2)){
				sum_x += x;
				n++;
			}
			if(!empty(srcImg1, x, srcImg1.height() / 2) && !empty(srcImg2, x, srcImg1.height() / 2)) {
				sum_x_overlap += x;
				overlap_n++;
			}
		}
	}
	else {
		for(int y = 0; y < srcImg1.height(); y++){
			if(!empty(srcImg1, srcImg1.width() / 2, y)){
				sum_y += y;
				n++;
			}
			if(!empty(srcImg1, srcImg1.width() / 2, y) && !empty(srcImg2, srcImg2.width() / 2, y)){
				sum_y_overlap += y;
				overlap_n++;
			}
		}
	}

	int min_len = (srcImg1.width() < srcImg1.height()) ? srcImg1.width() : srcImg1.height();

	int n_level = floor(log2(min_len));

	vector<CImg<float>> a_pyramid(n_level);
	vector<CImg<float>> b_pyramid(n_level);
	vector<CImg<float>> mask(n_level);

	// Initialize the base.
	a_pyramid[0] = srcImg1;
	b_pyramid[0] = srcImg2;
	mask[0] = CImg<float>(srcImg1.width(), srcImg1.height(), 1, 1, 0);

	if (srcImg1.width() > srcImg1.height()) {
		if (sum_x / n < sum_x_overlap / overlap_n) {
			for (int x = 0; x < sum_x_overlap / overlap_n; x++) {
				for (int y = 0; y < srcImg1.height(); y++) {
					mask[0](x, y) = 1;
				}
			}
		}
		else {
			for (int x = sum_x_overlap / overlap_n + 1; x < srcImg1.width(); x++) {
				for (int y = 0; y < srcImg1.height(); y++) {
					mask[0](x, y) = 1;
				}
			}
		}
	}
	else {
		if (sum_y / n < sum_y_overlap / overlap_n) {
			for (int x = 0; x < srcImg1.width(); x++) {
				for (int y = 0; y < sum_y_overlap / overlap_n; y++) {
					mask[0](x, y) = 1;
				}
			}
		}
		else {
			for (int x = 0; x < srcImg1.width(); x++) {
				for (int y = sum_y_overlap / overlap_n; y < srcImg1.height(); y++) {
					mask[0](x, y) = 1;
				}
			}
		}
	}
	// Down sampling a and b, building Gaussian pyramids.
	for (int i = 1; i < n_level; i++) {
		a_pyramid[i] = a_pyramid[i - 1].get_blur(2).get_resize(a_pyramid[i - 1].width() / 2, a_pyramid[i - 1].height() / 2, 1, a_pyramid[i - 1].spectrum(), 3);
		b_pyramid[i] = b_pyramid[i - 1].get_blur(2).get_resize(b_pyramid[i - 1].width() / 2, b_pyramid[i - 1].height() / 2, 1, b_pyramid[i - 1].spectrum(), 3);
		
		mask[i] = mask[i - 1].get_blur(2).get_resize(mask[i - 1].width() / 2, mask[i - 1].height() / 2, 1, mask[i - 1].spectrum(), 3);
	}
	
	// Building Laplacian pyramids.
	for (int i = 0; i < n_level - 1; i++) {
		a_pyramid[i] = a_pyramid[i] - a_pyramid[i + 1].get_resize(a_pyramid[i].width(), a_pyramid[i].height(), 1, a_pyramid[i].spectrum(), 3);
		b_pyramid[i] = b_pyramid[i] - b_pyramid[i + 1].get_resize(b_pyramid[i].width(), b_pyramid[i].height(), 1, b_pyramid[i].spectrum(), 3);
	}

	vector<CImg<float>> blend_pyramid(n_level);
	// Blending
	for (int i = 0; i < n_level; i++) {
		// Init blended pyramid as the first input image pyramid
		blend_pyramid[i] = CImg<float>(a_pyramid[i].width(), a_pyramid[i].height(), 1, a_pyramid[i].spectrum(), 0);
		for (int x = 0; x < blend_pyramid[i].width(); x++) {
			for (int y = 0; y < blend_pyramid[i].height(); y++) {
				for (int k = 0; k < blend_pyramid[i].spectrum(); k++) {
					blend_pyramid[i](x, y, k) = a_pyramid[i](x, y, k) * mask[i](x, y) + b_pyramid[i](x, y, k) * (1.0 - mask[i](x, y));
				}
			}
		}
	}
	// Resizing and filling empty spaces with black
	CImg<float> res = blend_pyramid[n_level - 1];
	for (int i = n_level - 2; i >= 0; i--) {
		res.resize(blend_pyramid[i].width(), blend_pyramid[i].height(), 1, blend_pyramid[i].spectrum(), 3);
		for (int x = 0; x < blend_pyramid[i].width(); x++) {
			for (int y = 0; y < blend_pyramid[i].height(); y++) {
				for (int k = 0; k < blend_pyramid[i].spectrum(); k++) {
					float temp = res(x, y, k) + blend_pyramid[i](x, y, k);
					if (temp > 255) {
						temp = 255;
					}
					else if (temp < 0) {
						temp = 0;
					}
					res(x, y, k) = temp;
				}
			}
		}
	}

	return res;
}