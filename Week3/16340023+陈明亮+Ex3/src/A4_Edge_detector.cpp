/*
*  Definition File for A4 paper edge detector
*/
#include "A4_Edge_detector.h"

Edge_detector::Edge_detector(CImg<float> in, float sigma){
	input = in;
	this->sigma = sigma;
	width = in.width();
	height = in.height();
}

// Turn colorful image into gray scale image
void Edge_detector::RGB_to_gray(){
	CImg<float> gray(input.width(), input.height(), 1, 1, 0);
	cimg_forXY(gray, x, y){
		gray(x, y) = (int)round((double)input(x, y, 0) * 0.299 + 
                                    (double)input(x, y, 1) * 0.587 + 
                                    (double)input(x, y, 2) * 0.114);
	}
	input = gray;
}

void Edge_detector::Print_lines_equations(int order, double k, double b){
	cout << "Line " << order << ": y = " << k << "x + " << b << endl;
}

// Maaping points in gray image into lines in hough space
void Edge_detector::Init_hough_space(){
	// Sobel operators constructions -- x and y directions
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
	CImg<float> gradient_x = gaussian_img;
	gradient_x = gradient_x.get_convolve(sobX);
	CImg<float> gradient_y = gaussian_img;
	gradient_y = gradient_y.get_convolve(sobY);
	// Init hough space, turning Rectangular System into Polar System
	int upBoundofP = (int)sqrt(width*width + height*height);
	CImg<float> hough(360, upBoundofP, 1, 1, 0);  // X for Radian, Y for length(P)
	this->hough_space = hough;
	// Loop to get hough space
	cimg_forXY(input, x, y){
		double lenOfGrad = sqrt(gradient_x(x, y)*gradient_x(x, y) + gradient_y(x, y)*gradient_y(x, y));
		// Check strength in Bound
		if(lenOfGrad > GRADIENT_BOUND){
			input(x, y) = lenOfGrad;
			cimg_forX(hough_space, alpha){
				double theta = ((double)alpha * cimg::PI) / 180;
				int P = (int)(x*cos(theta) + y*sin(theta));  // Length of line in Polar system
				// Judge Bound
				if(P >= 0 && P < upBoundofP){
					hough_space(alpha, P)++; // Adding a line in hough space(Polar System presentation) 
											 // for a point in input image
				}
			}
		}
	}
}

// Startup of the A4 paper lines and points detection and draw
void Edge_detector::Edge_detection(){
	double blue[] = {0, 0, 255};
	double red[] = {255, 0, 0};
	//RGB_to_gray();
	gaussian_img = input.get_blur(sigma);  // Use CImg lib method -- get_blur()
	Init_hough_space();  // Init hough space
	find_peaks();  // Find most votes peaks with hough space
	draw_lines(blue);  // Draw lines in vector Lines
	draw_points(red, 50);  // Draw points in vector 
	input.display();
}

// Use voting algotirhm to find peaks in hough space
void Edge_detector::find_peaks(){
	peaks.clear();
	int min = 0;
	int xMax = width - 1, yMax = height - 1;
	cimg_forXY(hough_space, alpha, p){
		if(hough_space(alpha, p) > VOTE_THRESHOLD){
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
					point temp = peaks[i];
					double dist = sqrt((temp.x - alpha)*(temp.x - alpha) + (temp.y - p)*(temp.y - p));
					if(dist < MIN_PEAK_DISTANCE){
						isPeak = false;
						// Adding lines count through a point
						if(temp.count < hough_space(alpha, p)){
							peaks[i] = point(alpha, p, hough_space(alpha, p));
						}
					}
				}
				if(isPeak){
					point target(alpha, p, hough_space(alpha, p)); // Push a ploar point into vector
																   // with line number count
					peaks.push_back(target);
				}
			}

		}
	}
}

CImg<unsigned char> Edge_detector::draw_lines(double color[3]){
	lines.clear();
	int count = 0;
	for(int i=0; i<peaks.size(); i++){
		int alpha = peaks[i].x, p = peaks[i].y;
		double theta = double(alpha) * cimg::PI / 180;
		double k = -cos(theta) / sin(theta);
		double b = double(p) / sin(theta);
		line temp(k, b);
		if(isfinite(b)){
			lines.push_back(temp);
			Print_lines_equations(++count, k, b);
		}
	}

	for(int i=0; i<lines.size(); i++){
		int xLow = (double)-lines[i].b / lines[i].k;
		int xUp = (double)(height - 1 - lines[i].b) / lines[i].k;
		int yLow = lines[i].b;
		int yUp = (width-1) * lines[i].k + lines[i].b;
		// Judging k's range
		if(abs(lines[i].k) > 1){
			input.draw_line(xLow, 0, xUp, height-1, color);
		}else {
			input.draw_line(0, yLow, width-1, yUp, color);
		}
	}
}

CImg<unsigned char> Edge_detector::draw_points(double color[3], int radius){
	intersections.clear();
	int count = 0;
	// Finding intersections of lines
	for(int i=0; i<lines.size(); i++){
		for(int j=i+1; j<lines.size(); j++){
			double x = (lines[j].b - lines[i].b) / -(lines[j].k - lines[i].k);
			double y = (lines[i].k * lines[j].b - lines[j].k * lines[i].b) / -(lines[j].k - lines[i].k);
			if(x >= 0 && x < width && y >= 0 && y < height){
				intersections.push_back(point(x, y, 0));
				cout << "Intersection Point " << ++count << ": x = " << x << ", y = " << y << endl;
			}
		}
	}
	// Draw points
	for(int i=0; i<intersections.size(); i++)
		input.draw_circle(intersections[i].x, intersections[i].y, radius, color);
}

void Edge_detector::usage(char* progName){
	cout << "USAGE: ./" << progName << " image_path sigma" << endl;
}