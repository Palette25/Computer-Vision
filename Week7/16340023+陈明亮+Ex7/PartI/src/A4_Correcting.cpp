/*
* Program: A4_Correcting.cpp
* Usage: Implement all methods in class
*/
#include "A4_Correcting.h"

// Image Transform
Image_Transformer::Image_Transformer(CImg<float>& input, CImg<float>& origin){
	this->srcImg = input;
	this->origin = origin;
	this->width = input.width();
	this->height = input.height();
}

// Init hough transform space
void Image_Transformer::initHoughSpace(){
	float x[3][3] = {{-1.0, 0.0, 1.0}, {-2.0, 0.0, 2.0}, {-1.0, 0.0, 1.0}};
	float y[3][3] = {{1.0, 2.0, 1.0}, {0.0, 0.0, 0.0}, {-1.0, -2.0, -1.0}};

	CImg<float> sobX(3, 3, 1, 1); CImg<float> sobY(3, 3, 1, 1);
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			sobX(i, j) = x[i][j];
			sobY(i, j) = y[i][j];
		}
	}
	// Use CImg lib methods to get x, y convolutions -- get_convolve()
	CImg<float> gradient_x = srcImg;
	gradient_x = gradient_x.get_convolve(sobX);
	CImg<float> gradient_y = srcImg;
	gradient_y = gradient_y.get_convolve(sobY);

	int upBound = (int)sqrt(width*width + height*height);
	this->houghSpace = CImg<float>(360, upBound, 1, 1, 0);
	// Loop to fiil hough space
	cimg_forXY(srcImg, x, y){
		double length = sqrt(gradient_x(x, y)*gradient_x(x, y) + gradient_y(x, y)*gradient_y(x, y));
		if(length > GRADIENT_BOUND){
			cimg_forX(houghSpace, alpha){
				double theta = ((double)alpha * cimg::PI) / 180;
				int P = (int)(x*cos(theta) + y*sin(theta));
				// Judge Bound
				if(P >= 0 && P < upBound){
					houghSpace(alpha, P)++;
				}
			}
		}
	}
}

void Image_Transformer::peakDetection(){
	int min = 0, xMax = width - 1, yMax = height - 1;
	cimg_forXY(houghSpace, alpha, p){
		//cout << houghSpace(alpha, p) << endl;
		if(houghSpace(alpha, p) > VOTE_THRESHOLD){
			bool isPeak = false;
			double theta = ((double)alpha * cimg::PI) / 180;
			// For a point in hough space, calculate x, y Bounds
			double xUp = ((double)p / cos(theta)) - min * tan(theta);
			double xLow = ((double)p / cos(theta)) - yMax * tan(theta);
			double yUp = ((double)p / sin(theta)) - min * (1 / tan(theta));
			double yLow = ((double)p / sin(theta)) - xMax * (1 / tan(theta));
			// Judge whether in the range
			if(xUp <= xMax && xUp >= min || xLow >= min && xLow <= xMax
					|| yUp <= yMax && yUp >= min || yLow >= min && yLow <= yMax){
				isPeak = true;
				for(int i=0; i<peaks.size(); i++){
					// Loop to figure out whether other peak points are too close
					Point temp = peaks[i];
					double dist = sqrt((temp.x - alpha)*(temp.x - alpha) + (temp.y - p)*(temp.y - p));
					if(dist < MIN_PEAK_DISTANCE){
						isPeak = false;
						// Adding lines count through a point
						if(temp.count < houghSpace(alpha, p)){
							peaks[i] = Point(alpha, p, houghSpace(alpha, p));
						}
					}
				}
				if(isPeak){
					Point target(alpha, p, houghSpace(alpha, p)); // Push a ploar point into vector
																   // with line number count
					peaks.push_back(target);
				}
			}
		}
	}
}

CImg<float> Image_Transformer::ImageTransform(){
	initHoughSpace();
	peakDetection();
	edgeAndPeakDetection();
	// Do perspective transform
	CImg<float> result = perspectiveTransform();
	result.display();
	return result;
}

bool sortByDist(Line a, Line b){
	return a.b < b.b;
}

bool sortByHough(Point a, Point b){
	return a.count > b.count;
}

void Image_Transformer::edgeAndPeakDetection(){
	CImg<float> temp = this->origin;
	// Search Lines
	for(int i=0; i<peaks.size(); i++){
		double theta = double(peaks[i].x) * cimg::PI / 180;
		float k = -cos(theta) / sin(theta), b = double(peaks[i].y) / sin(theta);
		lines.push_back(Line(k, b));
		cout << "Line: y = " << k << "x + " << b << endl;
	}
	// Serach intersactions
	vector<Point> tempIntersactions;
	for(int i=0; i<lines.size(); i++){
		for(int j=i+1; j<lines.size(); j++){
			float k0 = lines[i].k, b0 = lines[i].b;
			float k1 = lines[j].k, b1 = lines[j].b;

			float interX = (b1-b0) / (k0-k1);
			float interY = (k0*b1 - k1*b0) / (k0-k1);
			if(interX >= 0 && interX < width && interY >= 0 && interY < height){
				
				tempIntersactions.push_back(Point(interX, interY, 0));
				cout << "Point: " << interX << ' ' << interY << endl;
			}
		}
	}
	// Sort intersactions by order
	vector<Line> sortLines;
	for(int i=0; i<tempIntersactions.size(); i++){
		float subX = tempIntersactions[i].x - tempIntersactions[0].x;
		float subY = tempIntersactions[i].y - tempIntersactions[0].y;
		float dist = sqrt(subX*subX + subY*subY);
		sortLines.push_back(Line(i, dist));
	}
	sort(sortLines.begin(), sortLines.end(), sortByDist);
	unsigned char color[3] = {255, 0, 0};
	for(int i=0; i<tempIntersactions.size(); i++){
		temp.draw_circle(tempIntersactions[i].x, tempIntersactions[i].y, 30, color);
		intersactions.push_back(tempIntersactions[(int)sortLines[i].k]);
	}
	temp.display();
	temp.save("../result/edgeAndPoint.bmp");
}

vector<CImg<float> > Image_Transformer::getPerspectiveMatrix(){
	// Use standard A4 Paper size: 1240 X 1754
	CImg<float> A4(STANDARD_WIDTH, STANDARD_HEIGHT, 1, 3, 0);
	// Make Border
	vector<Point> boundPoints;
	Point leftUp(0, 0, 0);  Point rightUp(A4.width()-1, 0, 0);
	Point leftDown(0, A4.height()-1, 0);  Point rightDown(A4.width()-1, A4.height()-1, 0);
	boundPoints.push_back(leftUp);  boundPoints.push_back(rightUp);
	boundPoints.push_back(leftDown);  boundPoints.push_back(rightDown);

	CImg<float> y1(1, 3, 1, 1, 0), y2(1, 3, 1, 1, 0), y3(1, 3, 1, 1, 0), y4(1, 3, 1, 1, 0);
    CImg<float> c1(1, 3, 1, 1, 0), c2(1, 3, 1, 1, 0), c3(1, 3, 1, 1, 0), c4(1, 3, 1, 1, 0);
    CImg<float> A1(3, 3, 1, 1, 1), A2(3, 3, 1, 1, 1);

    for (int i = 0; i < 3; i++) {
        A1(0, i) = boundPoints[i].x; A1(1, i) = boundPoints[i].y;
        A2(0, i) = boundPoints[3-i].x; A2(1, i) = boundPoints[3-i].y;

        y1(0, i) = intersactions[i].x; y2(0, i) = intersactions[i].y;
        y3(0, i) = intersactions[3-i].x; y4(0, i) = intersactions[3-i].y;
    }
    c1 = y1.solve(A1); c2 = y2.solve(A1);
    c3 = y3.solve(A2); c4 = y4.solve(A2);

    // Make transform
    CImg<float> temptransform1(3, 3, 1, 1, 0), temptransform2(3, 3, 1, 1, 0);
    for (int i = 0; i < 3; i++) {
        temptransform1(i, 0) = c1(0, i);
        temptransform1(i, 1) = c2(0, i);

        temptransform2(i, 0) = c3(0, i);
        temptransform2(i, 1) = c4(0, i);
    }
    temptransform1(0, 2) = 0; temptransform1(1, 2) = 0; temptransform1(2, 2) = 1;
    temptransform2(0, 2) = 0; temptransform2(1, 2) = 0; temptransform2(2, 2) = 1;
    vector<CImg<float> > temptransform;
    temptransform.push_back(temptransform1);
    temptransform.push_back(temptransform2);
    return temptransform;
}

CImg<float> Image_Transformer::perspectiveTransform(){
	CImg<float> result(STANDARD_WIDTH, STANDARD_HEIGHT, 1, 3, 0);
	vector<CImg<float> > pTransform = getPerspectiveMatrix();

	CImg<float> y(1, 2, 1, 1, 0);
    CImg<float> c(1, 2, 1, 1, 0);
    CImg<float> A(2, 2, 1, 1, 1);
    A(0, 0) = 0;
    A(0, 1) = result.width() - 1;
    y(0, 0) = result.height() - 1;
    y(0, 1) = 0;
    c = y.solve(A);

    CImg<float> temp1(1, 3, 1, 1, 1), temp2(1, 3, 1, 1, 1);
    cimg_forXY(result, i, j) {
        temp1(0, 0) = i;
        temp1(0, 1) = j;

        double inProduct = i * c(0, 0) - j + c(0, 1);
        temp2 = inProduct >= 0 ? pTransform[0] * temp1 : pTransform[1] * temp1;
        temp2(0, 0) = temp2(0, 0) < 0 ? 0 : (temp2(0, 0) > width-1 ? width-1 : temp2(0, 0));
        temp2(0, 1) = temp2(0, 1) < 0 ? 0 : (temp2(0, 1) > height-1 ? height-1 : temp2(0, 1));

        result(i, j, 0) = origin(temp2(0, 0), temp2(0, 1), 0);
        result(i, j, 1) = origin(temp2(0, 0), temp2(0, 1), 1);
        result(i, j, 2) = origin(temp2(0, 0), temp2(0, 1), 2);
    }

	return result;
}