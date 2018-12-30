/*
* main.cpp -- Input A4 Test Paper and output segmented number images
*/
#include "utils.h"

#include "HoughTransform.h"
#include "ImageSegment.h"
#include "Partition.h"
#include "Preprocess.h"
#include "Warping.h"
#include <thread>

#define THRESHOLD 0.25
#define SIGMA 6.0
#define VOTING_THRES 60

void processImage(string& path, string& srcPath, bool flag, int seq);

int main(int argc, char* argv[]){
	string dataPath = "../testData/PartI/";
	// testFlag -- false for PartI test, true for PartII test
	bool testFlag = false;
	if(argc == 2){
		dataPath = "../testData/PartII/";
		testFlag = true;
		cout << "Start PartII A4 Paper dataset segmentation...." << endl;
	}else {
		cout << "Start PartI A4 Paper dataset segmentation...." << endl;
	}
	int testNum = 10 + testFlag * 81;
	// Start reading source images for segmentation
	for(int i=1; i<=testNum; i++){
		string path = to_string(i) + ".bmp";
		string srcPath = dataPath + path;
		cout << "Processing " + path << endl;
		processImage(path, srcPath, testFlag, i);
	}

	return 0;
}

void processImage(string& path, string& srcPath, bool flag, int seq){
	// Define output image path
	string outPath = "../output/PartI/";
	if(flag){
		outPath = "../output/PartII/";
	}
	string edgePath = outPath + "Edge_" + path;
	string segmentPath = outPath + "Segment_" + path;
	string pointPath = outPath + "Point_" + path;
	string resultPath = outPath + "Result_" + path;
	string cutPath = outPath + "Cut_" + path;

	// Begin Basic Image segmentation
	ImageSegmenter is(srcPath, SIGMA);
	CImg<unsigned char> segResult = is.getOutputImg();
	// Perform Hough Transform
	HoughTransformer htf(THRESHOLD, VOTING_THRES, segResult, 3, true);
	vector<HoughPos> houghResult = htf.result;

	// Perform transform warping
	CImg<unsigned char> srcImg(srcPath.c_str());
	Warper wr(houghResult, srcImg, 0);
	CImg<unsigned char> warpResult = wr.getResult();
	// Perform Paritition Cutting with Randon
	Partition pt(warpResult, seq);
	vector<vector<square> > parResult = pt.getSquare();
	// Preprocess for output slice images
	CImg<unsigned char> prResult = pt.getGrayImg();
	PreProcess pp(prResult, parResult, seq);
	
	ofstream out(pp.getSubDir() + "/points.txt");
	for(int i=0; i<4; i++){
		int x = (int)wr.srcPos[i][0],
			y = (int)wr.srcPos[i][1];
			out << x << " " << y << endl;
	}
	out.close();
	// Save Middle Images
	is.getBlockImg().save(segmentPath.c_str());
	is.getOutputImg().save(edgePath.c_str());
	htf.getResult().save(pointPath.c_str());
	wr.getResult().save(resultPath.c_str());
	pt.getDividedImg().save(cutPath.c_str());
}