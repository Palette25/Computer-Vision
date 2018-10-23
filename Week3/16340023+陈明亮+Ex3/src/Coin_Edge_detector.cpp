/*
*  Definition File of Coin Image detection
*/

#include "Coin_Edge_detector.h"
#include "CANNY.h"

bool comp(const pair<int, int>& p1, const pair<int, int>& p2){
	return p1.first > p2.first;
}

Edge_detector::Edge_detector(CImg<unsigned char> input, int min_radius, int max_radius, int votes_thres, int search_thres){
	this->input = input;
	this->min_radius = min_radius;
	this->max_radius = max_radius;
	this->votes_threshold = votes_thres;
	this->search_threshold = search_thres;
	width = input.width();
	height = input.height();
	decreaseCount = 0;
}

void Edge_detector::Edge_detection(){
	CImg<unsigned char> crImage(input);
    CANNY myCanny;
    crImage = myCanny.toGrayScale(crImage);
    unsigned char* grey = crImage._data;
    myCanny.canny(grey, crImage.width(), crImage.height(), 2.5f, 7.5f, 4.0f, 16);
    CImg<double> edge(myCanny.result, input.width(), input.height());
    edge.display();
	gaussian_img = edge;
	Init_hough_space();
	Hough_circle_transform();
	cout << "The total number of circles: " << circleNumber << endl;
	input.display();
}

void Edge_detector::RGB_to_gray(){
	CImg<unsigned char> gray(input.width(), input.height(), 1, 1);
	cimg_forXY(input, x, y){
		gray(x, y) = input(x, y, 0) * 0.299 + input(x, y, 1) * 0.587 + input(x, y ,2) * 0.114;
	}
	this->gray = gray;
}

// Use Circle Polar System to do Hough Circle Space Initialization
void Edge_detector::Init_hough_space(){
	int max_votes = 0, radius_gap = 2;
	// Begin from min accepting radius, increase with r_gap, finding circles in condition
	for(int i=min_radius; i<max_radius; i+=radius_gap){
		cout << i << endl;
		max_votes = 0;
		hough_space = CImg<float>(width, height, 1, 1, 0);
		hough_space.fill(0);
		cimg_forXY(hough_space, x, y){
			if(gaussian_img(x, y) > 0){
				// Start to vote with Polar System points
				for(int j=0; j<360; j++){
					double Sin = sin(2 * cimg::PI * j / 360);
					double Cos = cos(2 * cimg::PI * j / 360);
					int x1 = x - i * Cos;
					int y1 = y - i * Sin;
					// If in range
					if(x1 > 0 && x1 < width && y1 > 0 && y1 < height){
						hough_space(x1, y1)++;
					}
				}
			}
		}
		for(int j=0; j<width; j++){
			for(int k=0; k<height; k++){
				// Find the most votes number in each finding in hough space
				if(hough_space(j, k) > max_votes)
					max_votes = hough_space(j, k);
			}
		}
		votes.push_back(make_pair(max_votes, i));
	}
}

// According to hough space, find circles and centers
void Edge_detector::Hough_circle_transform(){
	// Sort the votes set
	circleNumber = 0;
	sort(votes.begin(), votes.end(), comp);
	for(int i=0; i<votes.size(); i++){
		cout << votes[i].first << ' ' << votes[i].second << endl;
		if(votes[i].first >= votes_threshold)
			++circleNumber;
	}
	// Loop to display each possible circles
	for(int i=0; i<circleNumber; i++){
		hough_space = CImg<float>(width, height);
		hough_space.fill(0);
		cimg_forXY(gaussian_img, x, y){
			if(gaussian_img(x, y) > 0){
				for(int j=0; j<360; j++){
					double Sin = sin((2 * cimg::PI * j) / 360);
					double Cos = cos((2 * cimg::PI * j) / 360);
					int x1 = x - votes[i].second * Cos;
					int y1 = y - votes[i].second * Sin;
					// Voting with each point
					if(x1 > 0 && x1 < width && y1 > 0 && y1 < height)
						hough_space(x1, y1)++;
				}
			}
		}
		cimg_forXY(hough_space, x, y){
			if(hough_space(x, y) != 0){
				centers.push_back(make_pair(x, y));
				final_center_votes.push_back(hough_space(x, y));
			}
		}
		draw_circle(votes[i].second);
		centers.clear();
		final_center_votes.clear();
	}
	circleNumber -= decreaseCount;
}

void Edge_detector::draw_circle(int radius){
	unsigned char blue[] = {0, 0, 255};
	unsigned char red[] = {255, 0, 0};
	// Copy a sort weight vector to find radius in order (big -> small)
	vector<int> temp = final_center_votes;
	sort(temp.begin(), temp.end(), greater<int>());
	// Loop to check whether circles are too close and draw correct circles
	int i = 0;
	for(i=0; i<search_threshold; i++){
		int currVotes = temp[i], j = 0;
		vector<int>::iterator it = find(final_center_votes.begin(), final_center_votes.end(), currVotes);
		int indexOfCurr =  it - final_center_votes.begin();
		int xOfCenter = centers[indexOfCurr].first, yOfCenter = centers[indexOfCurr].second;
		cout << xOfCenter << ' ' << yOfCenter << ' ' << radius << endl;

		if(xOfCenter+radius>=width || xOfCenter-radius<0 || yOfCenter+radius>=height || yOfCenter-radius<0){
			decreaseCount++;
			break;
		}
		// Loop to check other centers
		for(; j<result.size(); j++){
			double dist = sqrt(pow((result[j].first - xOfCenter), 2) + pow((result[j].second - yOfCenter), 2));
			// Too close will make two circles be the same 
			if(dist < min_radius)
				break;
		}
		if(j == result.size()){
			result.push_back(make_pair(xOfCenter, yOfCenter));
			input.draw_circle(xOfCenter, yOfCenter, radius, blue, 5.0f, 1);
			// Draw center of the circle
			input.draw_circle(xOfCenter, yOfCenter, 5, red);
			cout << "(x, y): (" << xOfCenter << ", " << yOfCenter << "), radius = " << radius << endl;
			break;
		}
	}
	if(i == search_threshold) decreaseCount++;
}

void Edge_detector::usage(char* progName){
	cout << "USAGE: ./" << progName << " image_path  sigma  min_radius max_radius votes_threshold search_threshold" << endl;
}