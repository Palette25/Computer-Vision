// MNIST Reader Definition
#include "MNISTProcessor.h"

int convertCharArrayToInt(unsigned char* array, int LengthOfArray) {
	if (LengthOfArray < 0) {
		return -1;
	}
	int result = static_cast<signed int>(array[0]);
	for (int i = 1; i < LengthOfArray; i++) {
		result = (result << 8) + array[i];
	}
	return result;
}


bool isImageDataFile(unsigned char* MagicNumber, int LengthOfArray) {
	int MagicNumberOfImage = convertCharArrayToInt(MagicNumber, LengthOfArray);
	if (MagicNumberOfImage == MAGICNUMBEROFIMAGE) {
		return true;
	}
	return false;
}

bool isLabelDataFile(unsigned char *MagicNumber, int LengthOfArray) {
	int MagicNumberOfLabel = convertCharArrayToInt(MagicNumber, LengthOfArray);
	if (MagicNumberOfLabel == MAGICNUMBEROFLABEL) {
		return true;
	}
	return false;
}

cv::Mat readData(std::fstream& DataFile, int NumberOfData, int DataSizeInBytes) {
	cv::Mat DataMat;
	// Read MNIST Image Data
	if (DataFile.is_open()) {
		int AllDataSizeInBytes = DataSizeInBytes * NumberOfData;
		unsigned char* TmpData = new unsigned char[AllDataSizeInBytes];
		DataFile.read((char *)TmpData, AllDataSizeInBytes);
		DataMat = cv::Mat(NumberOfData, DataSizeInBytes, CV_8UC1,
			TmpData).clone();
		delete[] TmpData;
		DataFile.close();
	}
	return DataMat;
}

// Resize MNIST Image
cv::Mat readImageData(std::fstream& ImageDataFile, int NumberOfImages) {
	int ImageSizeInBytes = 28 * 28;
	return readData(ImageDataFile, NumberOfImages, ImageSizeInBytes);
}

// Resize MNIST Label
cv::Mat readLabelData(std::fstream& LabelDataFile, int NumberOfLabel){
	int LabelSizeInBytes = 1;
	return readData(LabelDataFile, NumberOfLabel, LabelSizeInBytes);
}

cv::Mat readImages(std::string& FileName) {
	std::fstream File(FileName.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!File.is_open()){
		return cv::Mat();
	}
	MNISTImageFileHeader FileHeader;
	File.read((char *)(&FileHeader), sizeof(FileHeader));
	if (!isImageDataFile(FileHeader.MagicNumber, 4)){
		return cv::Mat();
	}
	int NumberOfImage = convertCharArrayToInt(FileHeader.NumberOfImages, 4);
	return readImageData(File, NumberOfImage);
}

// Read MNIST Labels
cv::Mat readLabels(std::string& FileName) {
	std::fstream File(FileName.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!File.is_open()) {
		return cv::Mat();
	}
	MNISTLabelFileHeader FileHeader;
	File.read((char *)(&FileHeader), sizeof(FileHeader));
	if (!isLabelDataFile(FileHeader.MagicNumber, 4)) {
		return cv::Mat();
	}
	int NumberOfImage = convertCharArrayToInt(FileHeader.NumberOfLabels, 4);
	return readLabelData(File, NumberOfImage);
}