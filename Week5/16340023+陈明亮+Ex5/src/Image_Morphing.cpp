/*
* Program: Image_Morphing.cpp
*/
#include "Image_Morphing.h"

Image_Morpher::Image_Morpher(int frameNum){
	this->srcImg = CImg<unsigned char>("../testData/1.bmp");
	this->destImg = CImg<unsigned char>("../testData/2.bmp");
	this->frameNum = frameNum;
}

void Image_Morpher::Morphing(){
	// Perform initializations
	initPoints();
	initTriangles();
	// Generate frames
	

	// Save frames
	saveFrames();
}

void Image_Morpher::initPoints(){
	// Init source image points
	string tempStr;
	int x_, y_;
	ifstream srcPoints_file;
	srcPoints_file.open("../facePoints/src_points.txt");
	if(srcPoints_file.is_open()){
		while(getline(srcPoints_file, tempStr)){
			stringstream ss(tempStr);
			ss >> x_ >> y_;
			this->srcPoints.push_back(Point(x_, y_));
		}
	}else {
		cout << "[Error] Cannot open file \'src_points.txt\'!" << endl;
		exit(1);
	}

	// Init dest image points
	ifstream destPoints_file;
	destPoints_file.open("../facePoints/dest_points.txt");
	if(destPoints_file.is_open()){
		while(getline(destPoints_file, tempStr)){
			stringstream ss(tempStr);
			ss >> x_ >> y_;
			this->destPoints.push_back(Point(x_, y_));
		}
	}else {
		cout << "[Error] Cannot open file \'dest_points.txt\'!" << endl;
		exit(1);
	}

	// Init middle frames points
	for(int i=0; i<frameNum; i++){
		vector<Point> midTemp;
		for(int j=0; j<srcPoints.size(); j++){
			float point_x = srcPoints[j].x + (i+1) / (frameNum+1) * (destPoints[j].x - srcPoints[j].x);
			float point_y = srcPoints[j].y + (i+1) / (frameNum+1) * (destPoints[j].y - srcPoints[j].y);
			midTemp.push_back(Point(point_x, point_y));
		}
		midFramePoints.push_back(midTemp);
	}
}

void Image_Morpher::initTriangles(){
	string tempStr;
	int x_, y_, z_;
	vector<Three_pair> store;
	// Init triangle lines
	ifstream lines_file;
	lines_file.open("../facePoints/lines.txt");
	if(lines_file.is_open()){
		while(getline(lines_file. tempStr)){
			stringstream ss(tempStr);
			ss >> x_ >> y_ >> z_;
			// Set source triangles list
			this->srcTriangles.push_back(Triangle(srcPoints[x_], srcPoints[y_], srcPoints[z_]));
			// Set dest triangles list
			this->destTriangles.push_back(Triangle(destPoints[x_], destPoints[y_], destPoints[z_]));
			store.push_back(Three_pair(x_, y_, z_));
		}
	}else {
		cout << "[Error] Cannot open file \'lines.txt\'!" << endl;
	}

	// Init middle frames triangles
	for(int i=0; i<frameNum; i++){
		vector<Triangle> midTemp;
		for(int j=0; j<store.size(); j++){
			midTemp.push_back(Triangle(midFramePoints[i][store[j].x], midFramePoints[i][store[j].y], midFramePoints[i][store[j].z]));
		}
		midFrameTriangles.push_back(midTemp);
	}
}

void Image_Morpher::saveFrames(){

}

// Judge whether a point is in the triangle
bool Image_Morpher::isInTriangle(Point po, Triangle tr){

}

// Get transform matrix
CImg<unsigned char> Image_Morpher::getTransformMatrix(Triangle tr1, Triangle tr2){

}