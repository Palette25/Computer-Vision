/*
* Program: Image_Morphing.cpp
*/

#include "Image_Morphing.h"


Image_Morpher::Image_Morpher(int frameNum){
	this->srcImg = CImg<float>("../testData/1.bmp");
	this->srcImg.resize(324, 400);
	this->destImg = CImg<float>("../testData/2.bmp");
	this->destImg.resize(324, 400);
	this->frameNum = frameNum;
}

void Image_Morpher::Morphing(){
	// Perform initializations
	initPoints();
	initTriangles();
	// Generate frames
	// Push first frame
	frameImgList.push_back(srcImg);

	for(int i=0; i<frameNum; i++){
		CImg<float> mid(destImg.width(), destImg.height(), 1, 3, 1);
		CImg<float> tmp(1, 3, 1, 1, 1);
		CImg<float> trans0(1, 3, 1, 1, 1), trans1(1, 3, 1, 1, 1);
		cimg_forXY(mid, x, y){
			Delaunay tempMesh = midFrameMesh[i];
			for(int j=0; j<tempMesh.m_Tris.size(); j++){
				int* v = tempMesh.m_Tris[j].v;
				Triangle midTri = Triangle(midFramePoints[i][v[0]], midFramePoints[i][v[1]], midFramePoints[i][v[2]]);
				if(isInTriangle(Point(x, y), midTri)){
					tmp(0, 0) = x;  tmp(0, 1) = y;
					// Transform to source image
					Triangle srcTri = Triangle(srcPoints[v[0]], srcPoints[v[1]], srcPoints[v[2]]), 
								destTri = Triangle(destPoints[v[0]], destPoints[v[1]], destPoints[v[2]]);
					trans0 = (getTransformMatrix(midTri, srcTri)) * tmp;
					// Transform to dest image
					trans1 = (getTransformMatrix(midTri, destTri)) * tmp;
					// Combinition
					float rate = (float)i / (frameNum-1);
					//cout << trans0(0, 0) << ' ' << trans0(0, 1) << ' ' << trans1(0, 0) << ' ' << trans1(0, 1) << endl;
					mid(x, y, 0) = (1-rate) * srcImg(trans0(0, 0), trans0(0, 1), 0) + rate * destImg(trans1(0, 0), trans1(0, 1), 0);
					mid(x, y, 1) = (1-rate) * srcImg(trans0(0, 0), trans0(0, 1), 1) + rate * destImg(trans1(0, 0), trans1(0, 1), 1);
					mid(x, y, 2) = (1-rate) * srcImg(trans0(0, 0), trans0(0, 1), 2) + rate * destImg(trans1(0, 0), trans1(0, 1), 2);
					break;
				}
			}
		}
		frameImgList.push_back(mid);
	}
	frameImgList.push_back(destImg);
	// Save frames
	saveFrames();
	drawStructedImg();
}

void Image_Morpher::initPoints(){
	Point p0(0, 0), p1(srcImg.width()-1, 0);
	Point p2(srcImg.width()-1, srcImg.height()-1), p3(0, srcImg.height()-1);

	srcPoints.push_back(p0); srcPoints.push_back(p1);
	srcPoints.push_back(p2); srcPoints.push_back(p3);
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

	destPoints.push_back(p0); destPoints.push_back(p1);
	destPoints.push_back(p2); destPoints.push_back(p3);
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
		float alpha = i / (float)(frameNum-1);
		for(int j=0; j<srcPoints.size(); j++){
			float point_x = (1 - alpha) * srcPoints[j].x + alpha * destPoints[j].x;
			float point_y = (1 - alpha) * srcPoints[j].y + alpha * destPoints[j].y;
			midTemp.push_back(Point(point_x, point_y));
		}
		midFramePoints.push_back(midTemp);
	}
}

void Image_Morpher::initTriangles(){
	point p0(0, 0, 0), p1(srcImg.width()-1, 0, 0);
	point p2(srcImg.width()-1, srcImg.height()-1, 0), p3(0, srcImg.height()-1, 0);

	string tempStr;
	int x_, y_, z_;
	// Init triangle lines
	Delaunay srcMesh(p0, p1, p2, p3);
	for(int i=4; i<srcPoints.size(); i++){
		srcMesh.AddPoint(srcPoints[i].x, srcPoints[i].y, 0);
	}
	// Droping triangles
	for(int i=0; i<srcMesh.m_Tris.size(); i++){
		triangle temp = srcMesh.m_Tris[i];
		point p0 = srcMesh.m_Pts[temp.v[0]], p1 = srcMesh.m_Pts[temp.v[1]], p2 = srcMesh.m_Pts[temp.v[2]];
		srcTriangles.push_back(Triangle(Point(p0.x, p0.y), Point(p1.x, p1.y), Point(p2.x, p2.y)));
	}
	Delaunay destMesh(p0, p1, p2, p3);
	for(int i=4; i<destPoints.size(); i++){
		destMesh.AddPoint(destPoints[i].x, destPoints[i].y, 0);
	}
	// Droping triangles
	for(int i=0; i<destMesh.m_Tris.size(); i++){
		triangle temp = destMesh.m_Tris[i];
		point p0 = destMesh.m_Pts[temp.v[0]], p1 = destMesh.m_Pts[temp.v[1]], p2 = destMesh.m_Pts[temp.v[2]];
		destTriangles.push_back(Triangle(Point(p0.x, p0.y), Point(p1.x, p1.y), Point(p2.x, p2.y)));
	}

	// Init middle frames triangles
	for(int i=0; i<frameNum; i++){
		vector<Triangle> midTemp;
		Delaunay midMesh(p0, p1, p2, p3);
		for(int j=4; j<midFramePoints[i].size(); j++){
			midMesh.AddPoint(midFramePoints[i][j].x, midFramePoints[i][j].y, 0);
		}
		midFrameMesh.push_back(midMesh);
		// Droping triangles
		for(int j=0; j<midMesh.m_Tris.size(); j++){
			triangle temp = midMesh.m_Tris[j];
			point p0 = midMesh.m_Pts[temp.v[0]], p1 = midMesh.m_Pts[temp.v[1]], p2 = midMesh.m_Pts[temp.v[2]];
			midTemp.push_back(Triangle(Point(p0.x, p0.y), Point(p1.x, p1.y), Point(p2.x, p2.y)));
		}
		midFrameTriangles.push_back(midTemp);
	}
}

void Image_Morpher::saveFrames(){
	string dir_name = "../frameImg/";
	for(int i=0; i<frameImgList.size(); i++){
		string file_name = to_string(i) + ".bmp";
		string res_name = dir_name + file_name;
		frameImgList[i].save_bmp(res_name.c_str());
	}
}


void Image_Morpher::drawStructedImg(){
	unsigned char white[3] = {255, 255, 255};
	for(int i=0; i<srcPoints.size(); i++){
		srcImg.draw_circle(srcPoints[i].x, srcPoints[i].y, 2, white);
	}
	srcImg.save("../frameImg/src.bmp");
	for(int i=0; i<destPoints.size(); i++){
		destImg.draw_circle(destPoints[i].x, destPoints[i].y, 2, white);
	}
	destImg.save("../frameImg/dest.bmp");
	for(int i=0; i<srcTriangles.size(); i++){
		srcImg.draw_line(srcTriangles[i].p1.x, srcTriangles[i].p1.y, srcTriangles[i].p2.x, srcTriangles[i].p2.y, white);
		srcImg.draw_line(srcTriangles[i].p1.x, srcTriangles[i].p1.y, srcTriangles[i].p3.x, srcTriangles[i].p3.y, white);
		srcImg.draw_line(srcTriangles[i].p3.x, srcTriangles[i].p3.y, srcTriangles[i].p2.x, srcTriangles[i].p2.y, white);
	}
	srcImg.save("../frameImg/srcTriangle.bmp");
	for(int i=0; i<destTriangles.size(); i++){
		destImg.draw_line(destTriangles[i].p1.x, destTriangles[i].p1.y, destTriangles[i].p2.x, destTriangles[i].p2.y, white);
		destImg.draw_line(destTriangles[i].p1.x, destTriangles[i].p1.y, destTriangles[i].p3.x, destTriangles[i].p3.y, white);
		destImg.draw_line(destTriangles[i].p3.x, destTriangles[i].p3.y, destTriangles[i].p2.x, destTriangles[i].p2.y, white);
	}
	destImg.save("../frameImg/destTriangle.bmp");
}

// Judge whether a point is in the triangle, using core-methods
bool Image_Morpher::isInTriangle(Point po, Triangle tr){
	// Judge whether C and P are in the same side of AB
	Point A = tr.p1, B = tr.p2, C = tr.p3;
	float x0 = B.x - A.x, x1 = C.x - A.x, x2 = po.x - A.x;
	float y0 = B.y - A.y, y1 = C.y - A.y, y2 = po.y - A.y;
	float across_ac = x0 * y1 - y0 * x1, across_cp = x0 * y2 - y0 * x2;
	bool flag_0 = ((across_ac * across_cp) >= 0);
	// Judge whether B and P are in the same side of CA
	x0 = A.x - C.x; x1 = B.x - C.x; x2 = po.x - C.x;
	y0 = A.y - C.y; y1 = B.y - C.y; y2 = po.y - C.y;
	float across_ab = x0 * y1 - y0 * x1, across_bp = x0 * y2 - y0 * x2;
	bool flag_1 = ((across_ab * across_bp) >= 0);
	// Judge whether A and P are in the same side of BC
	x0 = C.x - B.x; x1 = A.x - B.x; x2 = po.x - B.x;
	y0 = C.y - B.y; y1 = A.y - B.y; y2 = po.y - B.y;
	float across_bc = x0 * y1 - y0 * x1, across_ap = x0 * y2 - y0 * x2;
	bool flag_2 = ((across_bc * across_ap) >= 0);

	return flag_0 && flag_1 && flag_2;
}

// Get transform matrix
CImg<float> Image_Morpher::getTransformMatrix(Triangle tr1, Triangle tr2){
	CImg<float> store(3, 3, 1, 1, 1);
	CImg<float> y0(1, 3, 1, 1, 0), y1(1, 3, 1, 1, 0);
	CImg<float> trans0(1, 3, 1, 1, 0), trans1(1, 3, 1, 1, 0);
	CImg<float> result(3, 3, 1, 1, 0);
	// Initlialize the store matrixes
	store(0, 0) = tr1.p1.x;  store(0, 1) = tr1.p2.x;  store(0, 2) = tr1.p3.x;
	store(1, 0) = tr1.p1.y;  store(1, 1) = tr1.p2.y;  store(1, 2) = tr1.p3.y;
	// Initialize x-dir, y-dir after-triangle vectors
	y0(0, 0) = tr2.p1.x;  y0(0, 1) = tr2.p2.x;  y0(0, 2) = tr2.p3.x;
	y1(0, 0) = tr2.p1.y;  y1(0, 1) = tr2.p2.y;  y1(0, 2) = tr2.p3.y;
	// solve
	trans0 = y0.solve(store);
	trans1 = y1.solve(store);

	for(int i=0; i<3; i++){
		result(i, 0) = trans0(0, i);
		result(i, 1) = trans1(0, i);
	}
	result(2, 2) = 1;
	return result;
}