#include "NumberSegmentation.h"
#include <direct.h>

int lineColor[3]{ 255, 0, 0 };

ImageSegmentation::ImageSegmentation(const CImg<int>& _SrcImg) {
	SrcGrayImg = CImg<int>(_SrcImg._width, _SrcImg._height, 1, 1, 0);
	imgW = _SrcImg._width;
	imgH = _SrcImg._height;
	// To gray scale
	cimg_forXY(SrcGrayImg, x, y) {
		SrcGrayImg(x, y) = 0.299 * _SrcImg(x, y, 0) + 0.587 * _SrcImg(x, y, 1) + 0.114 * _SrcImg(x, y, 2);
	}
}

// Dilation in X-Y directions
int getDilationIntensityXY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (0 <= x + i && x + i < Img._width
					&& 0 <= y + j && y + j < Img._height) {
					if (i != -1 && j != -1 || i != 1 && j != 1 || i != 1 && j != -1 || i != -1 && j != 1)
						if (Img(x + i, y + j, 0) == 0) {
							intensity = 0;
							break;
						}
				}
			}
			if (intensity != 255)
				break;
		}
	}
	return intensity;
}

// Dilation in X direction
int getDilationIntensityX(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int i = -1; i <= 1; i++) {
			if (0 <= x + i && x + i < Img._width) {
				if (Img(x + i, y, 0) == 0) {
					intensity = 0;
					break;
				}
			}
		}
	}
	return intensity;
}

// Dilation in Y direction
int getDilationIntensityY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int j = -1; j <= 1; j++) {
			if (0 <= y + j && y + j < Img._height) {
				if (Img(x, y + j, 0) == 0) {
					intensity = 0;
					break;
				}
			}
		}
	}
	return intensity;
}

//X方向2个单位的负扩张，Y方向2个单位的正扩张
int getDilationIntensityXXYY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		int blackAccu = 0;

		for (int i = -2; i <= 2; i++) {
			if (0 <= y + i && y + i < Img._height) {
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
			if (0 <= x + i && x + i < Img._width) {
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

//X方向2个单位的负扩张，Y方向1个单位的正扩张
int getDilationIntensityXXY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {    //若中间点为白色
		int blackAccu = 0;

		for (int i = -1; i <= 1; i++) {
			if (0 <= y + i && y + i < Img._height) {    //垂直方向累加
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
		}
		for (int i = -2; i <= 2; i++) {
			if (0 <= x + i && x + i < Img._width) {     //水平方向累减
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

//X方向1个单位的负扩张，Y方向1个单位的正扩张
int getDilationIntensityXrY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {    //若中间点为白色
		int blackAccu = 0;

		for (int i = -1; i <= 1; i++) {
			if (0 <= y + i && y + i < Img._height) {    //垂直方向累加
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
			if (0 <= x + i && x + i < Img._width) {     //水平方向累减
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

void ImageSegmentation::processBinaryImage() {
	CImg<int> answer = CImg<int>(SrcGrayImg._width, SrcGrayImg._height, 1, 1, 0);

	cimg_forXY(answer, x, y) {
		int intensity = SrcGrayImg(x, y, 0);

		//先去掉黑色边缘
		if (x <= BoundaryRemoveGap || y <= BoundaryRemoveGap
			|| x >= imgW - BoundaryRemoveGap || y >= imgH - BoundaryRemoveGap) {
			answer(x, y, 0) = 255;
		}
		else {
			if (intensity < BinaryGap)
				answer(x, y, 0) = 0;
			else
				answer(x, y, 0) = 255;
		}
	}

	BinaryImg = answer;
}

// Startup of the whole image sementation process
void ImageSegmentation::numberSegmentationMainProcess(const string baseAddress) {
	if (_access(baseAddress.c_str(), 0) == -1)
		_mkdir(baseAddress.c_str());
	basePath = baseAddress + "/";

	findDividingLine();
	divideIntoBarItemImg();

	connectedRegionsTagging();
}

void ImageSegmentation::findDividingLine() {
	HistogramImage = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	DividingImg = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	int lineColor[3]{ 255, 0, 0 };

	cimg_forY(HistogramImage, y) {
		int blackPixel = 0;
		cimg_forX(BinaryImg, x) {
			HistogramImage(x, y, 0) = 255;
			HistogramImage(x, y, 1) = 255;
			HistogramImage(x, y, 2) = 255;
			DividingImg(x, y, 0) = BinaryImg(x, y, 0);
			DividingImg(x, y, 1) = BinaryImg(x, y, 0);
			DividingImg(x, y, 2) = BinaryImg(x, y, 0);
			if (BinaryImg(x, y, 0) == 0)
				blackPixel++;
		}

		cimg_forX(HistogramImage, x) {
			if (x < blackPixel) {
				HistogramImage(x, y, 0) = 0;
				HistogramImage(x, y, 1) = 0;
				HistogramImage(x, y, 2) = 0;
			}
		}

		// Judge peak point or not
		if (y > 0) {
			if (blackPixel <= YHistogramValleyMaxPixelNumber
				&& HistogramImage(YHistogramValleyMaxPixelNumber, y - 1, 0) == 0) {
				inflectionYPointSet.push_back(y);
			}
			else if (blackPixel > YHistogramValleyMaxPixelNumber
				&& HistogramImage(YHistogramValleyMaxPixelNumber, y - 1, 0) != 0) {
				inflectionYPointSet.push_back(y - 1);
			}
		}
	}

	divideLinePointSet.push_back(PointPos(0, -1));

	if (inflectionYPointSet.size() > 2) {
		for (int i = 1; i < inflectionYPointSet.size() - 1; i = i + 2) {
			int divideLinePointY = (inflectionYPointSet[i] + inflectionYPointSet[i + 1]) / 2;
			divideLinePointSet.push_back(PointPos(0, divideLinePointY));
		}
	}
	divideLinePointSet.push_back(PointPos(0, BinaryImg._height - 1));
}

vector<int> getInflectionPosXs(const CImg<int>& XHistogramImage) {
	vector<int> resultInflectionPosXs;
	vector<int> tempInflectionPosXs;
	int totalDist = 0, avgDist;
	int distNum = 0;
	cimg_forX(XHistogramImage, x) {
		if (x >= 1) {
			if (XHistogramImage(x, 0, 0) == 0 && XHistogramImage(x - 1, 0, 0) == 255) {
				tempInflectionPosXs.push_back(x - 1);
			}
			else if (XHistogramImage(x, 0, 0) == 255 && XHistogramImage(x - 1, 0, 0) == 0) {
				tempInflectionPosXs.push_back(x);
			}
		}
	}
	for (int i = 2; i < tempInflectionPosXs.size() - 1; i = i + 2) {
		int dist = tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
		if (dist <= 0)
			distNum--;
		totalDist += dist;
	}

	distNum += (tempInflectionPosXs.size() - 2) / 2;
	if (distNum == 0) {
		avgDist = 0;
	}
	else {
		avgDist = totalDist / distNum;
	}
	

	resultInflectionPosXs.push_back(tempInflectionPosXs[0]);
	for (int i = 2; i < tempInflectionPosXs.size() - 1; i = i + 2) {
		int dist = tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
		if (dist > avgDist * XHistogramValleyMaxPixelNumber) {
			resultInflectionPosXs.push_back(tempInflectionPosXs[i - 1]);
			resultInflectionPosXs.push_back(tempInflectionPosXs[i]);
		}
	}
	resultInflectionPosXs.push_back(tempInflectionPosXs[tempInflectionPosXs.size() - 1]);  //尾

	return resultInflectionPosXs;
}

vector<int> getDivideLineXofSubImage(const CImg<int>& subImg) {
	vector<int> InflectionPosXs;

	CImg<int> XHistogramImage = CImg<int>(subImg._width, subImg._height, 1, 3, 0);
	cimg_forX(subImg, x) {
		int blackPixel = 0;
		cimg_forY(subImg, y) {
			XHistogramImage(x, y, 0) = 255;
			XHistogramImage(x, y, 1) = 255;
			XHistogramImage(x, y, 2) = 255;
			if (subImg(x, y, 0) == 0)
				blackPixel++;
		}
		if (blackPixel >= XHistogramValleyMaxPixelNumber) {
			cimg_forY(subImg, y) {
				if (y < blackPixel) {
					XHistogramImage(x, y, 0) = 0;
					XHistogramImage(x, y, 1) = 0;
					XHistogramImage(x, y, 2) = 0;
				}
			}
		}
	}

	InflectionPosXs = getInflectionPosXs(XHistogramImage);
	cout << "Image Inflection Segmentation Points Size: " << InflectionPosXs.size() << endl;
	for (int i = 0; i < InflectionPosXs.size(); i++)
		XHistogramImage.draw_line(InflectionPosXs[i], 0, InflectionPosXs[i], XHistogramImage._height - 1, lineColor);
	
	vector<int> dividePosXs;
	dividePosXs.push_back(-1);
	if (InflectionPosXs.size() > 2) {
		for (int i = 1; i < InflectionPosXs.size() - 1; i = i + 2) {
			int divideLinePointX = (InflectionPosXs[i] + InflectionPosXs[i + 1]) / 2;
			dividePosXs.push_back(divideLinePointX);
		}
	}
	dividePosXs.push_back(XHistogramImage._width - 1);

	return dividePosXs;
}

vector<CImg<int>> getRowItemImgSet(const CImg<int>& lineImg, vector<int> _dividePosXset) {
	vector<CImg<int>> result;
	for (int i = 1; i < _dividePosXset.size(); i++) {
		int rowItemWidth = _dividePosXset[i] - _dividePosXset[i - 1];
		CImg<int> rowItemImg = CImg<int>(rowItemWidth, lineImg._height, 1, 1, 0);
		cimg_forXY(rowItemImg, x, y) {
			rowItemImg(x, y, 0) = lineImg(x + _dividePosXset[i - 1] + 1, y, 0);
		}
		result.push_back(rowItemImg);
	}

	return result;
}

void ImageSegmentation::divideIntoBarItemImg() {
	vector<PointPos> newDivideLinePointSet;

	for (int i = 1; i < divideLinePointSet.size(); i++) {
		int barHeight = divideLinePointSet[i].y - divideLinePointSet[i - 1].y;
		int blackPixel = 0;
		CImg<int> barItemImg = CImg<int>(BinaryImg._width, barHeight, 1, 1, 0);
		cimg_forXY(barItemImg, x, y) {
			barItemImg(x, y, 0) = BinaryImg(x, divideLinePointSet[i - 1].y + 1 + y, 0);
			if (barItemImg(x, y, 0) == 0)
				blackPixel++;
		}

		double blackPercent = (double)blackPixel / (double)(BinaryImg._width * barHeight);
		
		if (blackPercent > SubImgBlackPixelPercentage) {
			vector<int> dividePosXset = getDivideLineXofSubImage(barItemImg);
			vector<CImg<int>> rowItemImgSet = getRowItemImgSet(barItemImg, dividePosXset);

			for (int j = 0; j < rowItemImgSet.size(); j++) {
				subImageSet.push_back(rowItemImgSet[j]);
				newDivideLinePointSet.push_back(PointPos(dividePosXset[j], divideLinePointSet[i - 1].y));
			}

			if (i > 1) {
				HistogramImage.draw_line(0, divideLinePointSet[i - 1].y,
					HistogramImage._width - 1, divideLinePointSet[i - 1].y, lineColor);
				DividingImg.draw_line(0, divideLinePointSet[i - 1].y,
					HistogramImage._width - 1, divideLinePointSet[i - 1].y, lineColor);
			}
			for (int j = 1; j < dividePosXset.size() - 1; j++) {
				DividingImg.draw_line(dividePosXset[j], divideLinePointSet[i - 1].y,
					dividePosXset[j], divideLinePointSet[i].y, lineColor);
			}
		}
	}

	divideLinePointSet.clear();
	for (int i = 0; i < newDivideLinePointSet.size(); i++)
		divideLinePointSet.push_back(newDivideLinePointSet[i]);
}

void ImageSegmentation::connectedRegionsTagging() {
	for (int i = 0; i < subImageSet.size(); i++) {
		doDilationForEachBarItemImg(i);
		connectedRegionsTaggingOfBarItemImg(i);

		saveSingleNumberImageAndImglist(i);
	}

	ofstream predictImageListOutput(basePath + "predict_imagelist.txt");
	predictImageListOutput << imglisttxt.c_str();
	predictImageListOutput.close();
}

void ImageSegmentation::doDilationForEachBarItemImg(int barItemIndex) {
	CImg<int> answerXXY = CImg<int>(subImageSet[barItemIndex]._width, subImageSet[barItemIndex]._height, 1, 1, 0);
	cimg_forXY(subImageSet[barItemIndex], x, y) {
		int intensity = getDilationIntensityXXY(subImageSet[barItemIndex], x, y);
		answerXXY(x, y, 0) = intensity;
	}

	CImg<int> answerXXY2 = CImg<int>(answerXXY._width, answerXXY._height, 1, 1, 0);
	cimg_forXY(answerXXY, x, y) {
		int intensity = getDilationIntensityXXY(answerXXY, x, y);
		answerXXY2(x, y, 0) = intensity;
	}

	CImg<int> answerXY = CImg<int>(answerXXY2._width, answerXXY2._height, 1, 1, 0);
	cimg_forXY(answerXXY2, x, y) {
		int intensity = getDilationIntensityXY(answerXXY2, x, y);
		answerXY(x, y, 0) = intensity;
	}

	cimg_forXY(subImageSet[barItemIndex], x, y) {
		subImageSet[barItemIndex](x, y, 0) = answerXY(x, y, 0);
	}
}

void ImageSegmentation::connectedRegionsTaggingOfBarItemImg(int barItemIndex) {
	TagImage = CImg<int>(subImageSet[barItemIndex]._width, subImageSet[barItemIndex]._height, 1, 1, 0);
	tagAccumulate = -1;

	cimg_forX(subImageSet[barItemIndex], x)
		cimg_forY(subImageSet[barItemIndex], y) {
		if (x == 0 || y == 0) {
			int intensity = subImageSet[barItemIndex](x, y, 0);
			if (intensity == 0) {
				addNewClass(x, y, barItemIndex);
			}
		}
		else {
			int intensity = subImageSet[barItemIndex](x, y, 0);
			if (intensity == 0) {

				int minTag = Infinite;
				PointPos minTagPointPos(-1, -1);

				findMinTag(x, y, minTag, minTagPointPos, barItemIndex);

				if (minTagPointPos.x != -1 && minTagPointPos.y != -1) {
					mergeTagImageAndList(x, y - 1, minTag, minTagPointPos, barItemIndex);
					for (int i = -1; i <= 1; i++) {
						if (y + i < subImageSet[barItemIndex]._height)
							mergeTagImageAndList(x - 1, y + i, minTag, minTagPointPos, barItemIndex);
					}

					TagImage(x, y, 0) = minTag;
					PointPos cPoint(x + divideLinePointSet[barItemIndex].x + 1, y + divideLinePointSet[barItemIndex].y + 1);
					pointPosListSet[minTag].push_back(cPoint);

				}
				else {
					addNewClass(x, y, barItemIndex);
				}
			}
		}
	}
}

void ImageSegmentation::addNewClass(int x, int y, int barItemIndex) {
	tagAccumulate++;
	TagImage(x, y, 0) = tagAccumulate;
	classTagSet.push_back(tagAccumulate);
	list<PointPos> pList;
	PointPos cPoint(x + divideLinePointSet[barItemIndex].x + 1, y + divideLinePointSet[barItemIndex].y + 1);
	pList.push_back(cPoint);
	pointPosListSet.push_back(pList);
}

void ImageSegmentation::findMinTag(int x, int y, int &minTag, PointPos &minTagPointPos, int barItemIndex) {
	if (subImageSet[barItemIndex](x, y - 1, 0) == 0) {     //正上
		if (TagImage(x, y - 1, 0) < minTag) {
			minTag = TagImage(x, y - 1, 0);
			minTagPointPos.x = x;
			minTagPointPos.y = y - 1;
		}
	}
	for (int i = -1; i <= 1; i++) {        //左上、左中、左下
		if (y + i < subImageSet[barItemIndex]._height) {
			if (subImageSet[barItemIndex](x - 1, y + i, 0) == 0 && TagImage(x - 1, y + i, 0) < minTag) {
				minTag = TagImage(x - 1, y + i, 0);
				minTagPointPos.x = x - 1;
				minTagPointPos.y = y + i;
			}
		}
	}
}

void ImageSegmentation::mergeTagImageAndList(int x, int y, const int minTag, const PointPos minTagPointPos, int barItemIndex) {
	if (subImageSet[barItemIndex](x, y, 0) == 0) {
		int tagBefore = TagImage(x, y, 0);
		if (tagBefore != minTag) {
			list<PointPos>::iterator it = pointPosListSet[tagBefore].begin();
			for (; it != pointPosListSet[tagBefore].end(); it++) {
				TagImage((*it).x - divideLinePointSet[barItemIndex].x - 1, (*it).y - divideLinePointSet[barItemIndex].y - 1, 0) = minTag;
			}
			pointPosListSet[minTag].splice(pointPosListSet[minTag].end(), pointPosListSet[tagBefore]);
		}
	}
}


CImg<int> do_Y_Erosion(CImg<int>& Img) {
	CImg<int> result = CImg<int>(Img._width, Img._height, 1, 1, 0);
	cimg_forXY(Img, x, y) {
		result(x, y, 0) = Img(x, y, 0);
		if (Img(x, y, 0) == 255) {
			if (y - 1 >= 0) {
				if (Img(x, y - 1, 0) == 0)
					result(x, y, 0) = 0;
			}

			if (y + 1 < Img._height) {
				if (Img(x, y + 1, 0) == 0)
					result(x, y, 0) = 0;
			}
		}
	}
	return result;
}

void ImageSegmentation::saveSingleNumberImageAndImglist(int barItemIndex) {
	int totalBlacks = 0, numberCount = 0;
	for (int i = 0; i < pointPosListSet.size(); i++) {
		if (pointPosListSet[i].size() != 0) {
			totalBlacks += pointPosListSet[i].size();
			numberCount++;
		}
	}
	int avgBlacks = totalBlacks / numberCount;

	for (int i = 0; i < pointPosListSet.size(); i++) {
		if (pointPosListSet[i].size() != 0 && pointPosListSet[i].size() > avgBlacks * NumberImageBlackPixelPercentage) {
			int xMin, xMax, yMin, yMax;
			getBoundingOfSingleNum(i, xMin, xMax, yMin, yMax);

			int width = xMax - xMin;
			int height = yMax - yMin;

			int imgSize = (width > height ? width : height) + SingleNumberImgBoundary * 2;
			CImg<int> singleNum = CImg<int>(imgSize, imgSize, 1, 1, 0);

			list<PointPos>::iterator it = pointPosListSet[i].begin();
			for (; it != pointPosListSet[i].end(); it++) {
				int x = (*it).x;
				int y = (*it).y;
				int singleNumImgPosX, singleNumImgPosY;
				if (height > width) {
					singleNumImgPosX = (x - xMin) + (imgSize - width) / 2;
					singleNumImgPosY = (y - yMin) + SingleNumberImgBoundary;
				}
				else {
					singleNumImgPosX = (x - xMin) + SingleNumberImgBoundary;
					singleNumImgPosY = (y - yMin) + (imgSize - height) / 2;
				}
				singleNum(singleNumImgPosX, singleNumImgPosY, 0) = 255;
			}

			singleNum = do_Y_Erosion(singleNum);

			string postfix = ".bmp";
			char shortImgName[200];
			sprintf_s(shortImgName, "%d%s\n", imageOrder, postfix.c_str());
			imglisttxt += string(shortImgName);

			char addr[200];
			sprintf_s(addr, "output/singleNumber/2_%d%s", imageOrder++, postfix.c_str());
			singleNum.save(addr);

			pointPosListSetForDisplay.push_back(pointPosListSet[i]);
		}
	}
	imglisttxt += "*\n";

	classTagSet.clear();
	for (int i = 0; i < pointPosListSet.size(); i++) {
		pointPosListSet[i].clear();
	}
	pointPosListSet.clear();
}

CImg<int> ImageSegmentation::getHistogramImage() {
	return HistogramImage;
}

CImg<int> ImageSegmentation::getImageWithDividingLine() {
	return DividingImg;
}

void ImageSegmentation::getBoundingOfSingleNum(int listIndex, int& xMin, int& xMax, int& yMin, int& yMax) {
	xMin = yMin = Infinite;
	xMax = yMax = -1;

	if (!pointPosListSet.empty()) {
		list<PointPos>::iterator it = pointPosListSet[listIndex].begin();
		for (; it != pointPosListSet[listIndex].end(); it++) {
			int x = (*it).x;
			int y = (*it).y;
			xMin = x < xMin ? x : xMin;
			yMin = y < yMin ? y : yMin;
			xMax = x > xMax ? x : xMax;
			yMax = y > yMax ? y : yMax;
		}
	}
	else {
		list<PointPos>::iterator it = pointPosListSetForDisplay[listIndex].begin();
		for (; it != pointPosListSetForDisplay[listIndex].end(); it++) {
			int x = (*it).x;
			int y = (*it).y;
			xMin = x < xMin ? x : xMin;
			yMin = y < yMin ? y : yMin;
			xMax = x > xMax ? x : xMax;
			yMax = y > yMax ? y : yMax;
		}
	}
}

CImg<int> ImageSegmentation::getNumberDividedCircledImg() {
	int lineColor[3]{ 255, 0, 0 };

	CImg<int> answer = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	cimg_forXY(answer, x, y) {
		answer(x, y, 0) = 255;
		answer(x, y, 1) = 255;
		answer(x, y, 2) = 255;
	}
	
	for (int i = 0; i < pointPosListSetForDisplay.size(); i++) {
		if (pointPosListSetForDisplay[i].size() != 0) {
			//先绘制数字
			list<PointPos>::iterator it = pointPosListSetForDisplay[i].begin();
			for (; it != pointPosListSetForDisplay[i].end(); it++) {
				for (int k = 0; k < 3; k++) {
					answer((*it).x, (*it).y, k) = 0;
				}
			}

			//找到数字的包围盒
			int xMin, xMax, yMin, yMax;
			getBoundingOfSingleNum(i, xMin, xMax, yMin, yMax);

			//然后绘制包围盒
			answer.draw_line(xMin, yMin, xMax, yMin, lineColor);
			answer.draw_line(xMin, yMin, xMin, yMax, lineColor);
			answer.draw_line(xMin, yMax, xMax, yMax, lineColor);
			answer.draw_line(xMax, yMin, xMax, yMax, lineColor);
		}
	}

	return answer;
}