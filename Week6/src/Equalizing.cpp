#include "Equalizing.h"

Equalizater::Equalizater(CImg<unsigned char>& src){
	this->srcImg = src;
	this->dstImg = CImg<unsigned char>(src.width(), src.height(), src.depth(), 3, 0);
}

CImg<unsigned char> Equalizater::Equalization(){
	// Calculate YCbCr-Color hist
	CImg<unsigned char> temp = dstImg;
	cimg_forXY(temp, x, y){
		float Y = 0.299 * (float)srcImg(x, y, 0) + 0.857 * (float)srcImg(x, y, 1) + 0.114 * (float)srcImg(x, y, 2);
        float Cb = 128.0 - 0.168736 * (float)srcImg(x, y, 0) - 0.331264 * (float)srcImg(x, y, 1) + 0.5 * (float)srcImg(x, y, 2);
        float Cr = 128.0 + 0.5 * (float)srcImg(x, y, 0) - 0.418688 * (float)srcImg(x, y, 1) - 0.081312 * (float)srcImg(x, y, 2);
		temp(x, y, 0) = Y >= 0 ? (Y <= 255 ? Y : 255) : 0;
		temp(x, y, 1) = Cb >= 0 ? (Cb <= 255 ? Cb : 255) : 0;
		temp(x, y, 2) = Cr >= 0 ? (Cr <= 255 ? Cr : 255) : 0;
	}
	dstImg = temp;
	// Creating hist
	for(int i=0; i<256; i++){
		hist.push_back(0);
	}
	cimg_forXY(temp, x, y){
		hist[temp(x, y, 0)]++;
	}

	float thist[256] = {0};
	for(int i=0; i<256; i++){
		thist[i] = (float)hist[i] / (temp.width() * temp.height());
	}
	// Hist Mapping
	int mapHist[256] = {0};
	float sum = 0;
	for(int i=0; i<256; i++){
		sum += thist[i];
		mapHist[i] = round(255 * sum);
	}
	cimg_forXY(dstImg, x, y){
		int tt = temp(x, y, 0);
		int index = tt >= 0 ? (tt <= 255 ? tt : 255) : 0;
		dstImg(x, y, 0) = mapHist[index];
	}
	cimg_forXY(dstImg, x, y){
        float R = (float)dstImg(x, y, 0) + 1.402 * ((float)dstImg(x, y, 2) - 128.0);
        float G = (float)dstImg(x, y, 0) - 0.34414 * ((float)dstImg(x, y, 1) - 128.0) - 0.71414 * ((float)dstImg(x, y, 2) - 128.0);
        float B = (float)dstImg(x, y, 0) + 1.772 * ((float)dstImg(x, y, 1) - 128.0);
        dstImg(x, y, 0) = R > 0 ? (R < 256 ? R : 255) : 0;
        dstImg(x, y, 1) = G > 0 ? (G < 256 ? G : 255) : 0;
        dstImg(x, y, 2) = B > 0 ? (B < 256 ? B : 255) : 0;
    }
	// Combine original image with the result image, to make light equal
	// Step 1. RGB -> YCbCr
	CImg<float> temp1(srcImg.width(), srcImg.height(), 1, 3, 0);
	cimg_forXY(temp1, x, y){
		float Y = 0.299 * (float)srcImg(x, y, 0) + 0.857 * (float)srcImg(x, y, 1) + 0.114 * (float)srcImg(x, y, 2);
        float Cb = 128.0 - 0.168736 * (float)srcImg(x, y, 0) - 0.331264 * (float)srcImg(x, y, 1) + 0.5 * (float)srcImg(x, y, 2);
        float Cr = 128.0 + 0.5 * (float)srcImg(x, y, 0) - 0.418688 * (float)srcImg(x, y, 1) - 0.081312 * (float)srcImg(x, y, 2);
		temp1(x, y, 0) = Y >= 0 ? (Y <= 255 ? Y : 255) : 0;
		temp1(x, y, 1) = Cb >= 0 ? (Cb <= 255 ? Cb : 255) : 0;
		temp1(x, y, 2) = Cr >= 0 ? (Cr <= 255 ? Cr : 255) : 0;
	}

	CImg<float> temp2(srcImg.width(), srcImg.height(), 1, 3, 0);
	cimg_forXY(temp2, x, y){
		float Y = 0.299 * (float)dstImg(x, y, 0) + 0.857 * (float)dstImg(x, y, 1) + 0.114 * (float)dstImg(x, y, 2);
        float Cb = 128.0 - 0.168736 * (float)dstImg(x, y, 0) - 0.331264 * (float)dstImg(x, y, 1) + 0.5 * (float)dstImg(x, y, 2);
        float Cr = 128.0 + 0.5 * (float)dstImg(x, y, 0) - 0.418688 * (float)dstImg(x, y, 1) - 0.081312 * (float)dstImg(x, y, 2);
		temp2(x, y, 0) = Y >= 0 ? (Y <= 255 ? Y : 255) : 0;
		temp2(x, y, 1) = Cb >= 0 ? (Cb <= 255 ? Cb : 255) : 0;
		temp2(x, y, 2) = Cr >= 0 ? (Cr <= 255 ? Cr : 255) : 0;
	}
	// Step 2. Combined YCbCr Result -> RGB
	cimg_forXY(dstImg, x, y){
		temp1(x, y, 0) = temp1(x, y, 0) * 3.0f / 4.0f + temp2(x, y, 0) / 4.0f;
		float R = (float)temp1(x, y, 0) + 1.402 * ((float)temp1(x, y, 2) - 128.0);
        float G = (float)temp1(x, y, 0) - 0.34414 * ((float)temp1(x, y, 1) - 128.0) - 0.71414 * ((float)temp1(x, y, 2) - 128.0);
        float B = (float)temp1(x, y, 0) + 1.772 * ((float)temp1(x, y, 1) - 128.0);
		srcImg(x, y, 0) = R >= 0 ? (R <= 255 ? R : 255) : 0;
		srcImg(x, y, 1) = G >= 0 ? (G <= 255 ? G : 255) : 0;
		srcImg(x, y, 2) = B >= 0 ? (B <= 255 ? B : 255) : 0;
	}

	return srcImg;
}