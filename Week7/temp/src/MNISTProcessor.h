#ifndef MNIST_H
#define MNIST_H

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;

struct MNISTImageFileHeader
{
	unsigned char MagicNumber[4];
	unsigned char NumberOfImages[4];
	unsigned char NumberOfRows[4];
	unsigned char NumberOfColums[4];
};

struct MNISTLabelFileHeader
{
	unsigned char MagicNumber[4];
	unsigned char NumberOfLabels[4];
};

const int MAGICNUMBEROFIMAGE = 2051;
const int MAGICNUMBEROFLABEL = 2049;

int convertCharArrayToInt(unsigned char* array, int LengthOfArray);

bool isImageDataFile(unsigned char* MagicNumber, int LengthOfArray);

bool isLabelDataFile(unsigned char* MagicNumber, int LengthOfArray);

cv::Mat readData(std::fstream& DataFile, int NumberOfData, int DataSizeInBytes);

cv::Mat readImageData(std::fstream& ImageDataFile, int NumberOfImages);

cv::Mat readLabelData(std::fstream& LabelDataFile, int NumberOfLabel);

cv::Mat readImages(std::string& FileName);

cv::Mat readLabels(std::string& FileName);

#endif // MNIST_H