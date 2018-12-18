#ifndef NUMBER_SEGMENTATION_H
#define NUMBER_SEGMENTATION_H

#include "CImg.h"
#include <vector>
#include <list>
#include <iostream>
#include "fstream"
using namespace std;
using namespace cimg_library;

#define BinaryGap 135            // Image Binary Gap
#define BoundaryRemoveGap 7      // Image Border Remove Gap
#define Infinite 249480 

#define SingleNumberImgBoundary 5
#define YHistogramValleyMaxPixelNumber 0
#define XHistogramValleyMaxPixelNumber 4
#define XHistogramLargerScaleThanAvgDist 3

#define SubImgBlackPixelPercentage 0.001
#define NumberImageBlackPixelPercentage 0.35

struct PointPos {
	int x;
	int y;
	PointPos() : x(-1), y(-1) {}
	PointPos(int posX, int posY) : x(posX), y(posY) {}
};

class ImageSegmentation {
public:
	ImageSegmentation(const CImg<int>& _SrcImg);
	~ImageSegmentation(){};

	void processBinaryImage();

	// Main splited single number images process
	void numberSegmentationMainProcess(const string baseAddress);
	// Return final splited image count
	int getImageCount() {
		return imageOrder;
	}
	// Produce two directions histograms
	CImg<int> getHistogramImage();            
	// Produce splited line area     
	CImg<int> getImageWithDividingLine();
	// Produce single number window images
	CImg<int> getNumberDividedCircledImg();

private:
	// Find dividing lines in two directions
	void findDividingLine();
	// Divide images into rows and cols line solited
	void divideIntoBarItemImg();
	// Dilation
	void doDilationForEachBarItemImg(int barItemIndex);
	// Connect Region Tagging
	void connectedRegionsTagging();
	void connectedRegionsTaggingOfBarItemImg(int barItemIndex);

	void addNewClass(int x, int y, int barItemIndex);

	void findMinTag(int x, int y, int &minTag, PointPos &minTagPointPos, int barItemIndex);

	void mergeTagImageAndList(int x, int y, const int minTag, const PointPos minTagPointPos, int barItemIndex);

	void saveSingleNumberImageAndImglist(int barItemIndex);

	void getBoundingOfSingleNum(int listIndex, int& xMin, int& xMax, int& yMin, int& yMax);


private:
	CImg<int> SrcGrayImg, BinaryImg, TagImage, HistogramImage, DividingImg;
	vector<CImg<int>> subImageSet;
	int imgW, imgH;

	vector<int> inflectionYPointSet;
	vector<PointPos> divideLinePointSet;

	int tagAccumulate = -1;
	int imageOrder = 0;

	vector<int> classTagSet;
	vector<list<PointPos>> pointPosListSet;
	vector<list<PointPos>> pointPosListSetForDisplay;

	string basePath;
	string imglisttxt = "";
};

#endif