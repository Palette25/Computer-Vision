#include "HoughTransform.h"

HoughTransformer::HoughTransformer(double thres, double peakDis, CImg<unsigned char>& input, double fitDis, bool flag){
	// Flag decides randon transform or not
	if(flag == true){
        this->edgeImg = input;
        // to Colorful Image
        CImg<unsigned char> tmp(input.width(), input.height(), 1, 3, 0);
        cimg_forXY(tmp, x, y){
            if(input(x, y) == 255){
            	tmp(x, y, 0) = tmp(x, y, 1) = tmp(x, y, 2) = 255;
            }
        }
        this->lineImg = tmp;
        this->houghSpace = initHoughSpace();
        int max = 0, min = 10000000;
        cimg_forXY(houghSpace, x, y){
            if(houghSpace(x, y) > max){
                max = houghSpace(x, y);
            }
            if(houghSpace(x, y) < min){
                min = houghSpace(x, y);
            }
        }
        double threshold = max * thres;
        // Find Image's peaks and lines 
        findPeaks(threshold, peakDis);
        getLines();
        this->intersactionImg = this->lineImg;
        getIntersactions();
    } else{
        // Randon Transforming
        this->edgeImg = input;
        int bor = 10;
        cimg_forXY(this->edgeImg, x, y){
            if(x <= bor || y <= bor || x >= this->edgeImg.width() - bor || y >= this->edgeImg.height() - bor){
                this->edgeImg(x, y) = 0;
            }
        }
        // Ninty loops count
        int count = 90;
        while(count--){
            CImg<unsigned char>  tmp(this->edgeImg.width(), this->edgeImg.height(), 1, 1, 0);
            cimg_forXY(this->edgeImg, x, y){
                bool flag = false;
                for(int i = x - 1; i < x + 2; i++){
                    if(i < 0 || i >= this->edgeImg.width()) 
                    	continue;
                    if(this->edgeImg(i, y) != 0){
                        flag = true;
                        break;
                    }
                }
                if(flag) tmp(x, y) = 255;
            }
            this->edgeImg = tmp;
        }
       	// Remained ten loops count
        count = 10;
        while(count--){
            CImg<unsigned char> tmp(this->edgeImg.width(), this->edgeImg.height(), 1, 1, 0);
            cimg_forXY(this->edgeImg, x, y){
                bool flag = true;
                for(int i = y - 1; i < y + 2; i++){
                    if(i < 0 || i >= this->edgeImg.height()) 
                    	continue;
                    if(this->edgeImg(x, i) == 0){
                        flag = false;
                    }
                }
                if(flag) tmp(x, y) = 255;
            }
            this->edgeImg = tmp;
        }
        int diagonal = sqrt(this->edgeImg.width() * this->edgeImg.width() + this->edgeImg.height() * this->edgeImg.height());
        CImg<int> hough(DIVIDER * 4, diagonal, 1, 1, 0);
        cimg_forXY(this->edgeImg, x, y){
            if (this->edgeImg(x, y) == 255){
                cimg_forX(hough, alpha){
                    double theta = ((double)(alpha)* 0.5 * cimg::PI) / DIVIDER;
                    int r = round((double)x * cos(theta) + (double)y * sin(theta));
                    if (r >= 0 && r < diagonal){
                        hough(alpha, r)++;
                    }
                }
            }
        }
        this->houghSpace = hough;
        int max = 0;
        int pos = 0;
        cimg_forXY(this->houghSpace, x, y){
            if(this->houghSpace(x, y) > max){
                max = this->houghSpace(x, y);
                pos = x;
            }
        }
        if(max == 0) theta = 90.0;
        else theta = (double)pos / 2.0;
    }
}

CImg<int> HoughTransformer::initHoughSpace(){
	// Calculating diagonal
    int diagonal = sqrt(this->edgeImg.width() * this->edgeImg.width() + this->edgeImg.height() * this->edgeImg.height());
    CImg<int> hough(DIVIDER * 2, diagonal, 1, 1, 0);
    cimg_forXY(this->edgeImg, x, y){
        if (this->edgeImg(x, y) == 255){
            cimg_forX(hough, alpha){
                // Space transform
                double theta = ((double)(alpha) * cimg::PI) / DIVIDER;
                int r = round((double)x * cos(theta) + (double)y * sin(theta));
                // Hough Voting
                if (r >= 0 && r < diagonal){
                    hough(alpha, r)++;
                }
            }
        }
    }
    return hough;
}

void HoughTransformer::findPeaks(double thres, double dist){
    cimg_forXY(this->houghSpace, alpha, r){
        // Decide peaks according to voting number
        if (this->houghSpace(alpha, r) > thres){
            HoughPos tmp(alpha, r, this->houghSpace(alpha, r));
            bool flag = true;
            for (int i = 0; i < peaks.size(); i++){
                if (abs(peaks[i].y - r) < this->houghSpace.height() / 20 && 
                		(abs(peaks[i].x - alpha) < 20 || abs(peaks[i].x - alpha) > this->houghSpace.width() - 20 ||
                     		abs(abs(peaks[i].x - alpha) - 180) < 10)) {
                    	flag = false;
	                    if (this->houghSpace(alpha, r) > peaks[i].val){
	                        peaks[i].x = tmp.x;
	                        peaks[i].y = tmp.y;
	                        peaks[i].val = tmp.val;
	                    }
                }else {
                    continue;
                }
            }
            // Judging peak flag
            if (flag){
                peaks.push_back(tmp);
            }
        }
    }
}

// Getter methods
void HoughTransformer::getLines(){
	for(int i = 0; i < peaks.size(); i++){
        cimg_forXY(this->lineImg, x, y){
            double thet = ((double)peaks[i].x * cimg::PI) / 180.0;
            int p = (int)(x *cos(thet) + y * sin(thet));
            if(abs(p - peaks[i].y) < 2) this->lineImg(x, y, 2) = 255;
        }
    }
}

void HoughTransformer::getIntersactions(){
	unsigned char yellow[3] = {255, 255, 0};
    for(int i = 0; i < peaks.size(); i ++){
        for(int j = i + 1; j < peaks.size(); j ++){
            int a1 = peaks[i].x;
            int p1 = peaks[i].y;
            double theta1 = ((double)a1 * cimg::PI) / 180;
            int a2 = peaks[j].x;
            int p2 = peaks[j].y;
            double theta2 = ((double)a2 * cimg::PI) / 180;
            cimg_forXY(intersactionImg, x, y) {
                if(intersactionImg(x, y, 0) == 255 && intersactionImg(x, y, 1) == 255 && intersactionImg(x, y, 2) == 0) {
                    continue;
                }
                if(p1 == (int)round(y * sin(theta1)) + (int)round(x * cos(theta1))) {
                    bool flag = false;
                    for(int xb = x - 1; xb < x + 2; xb++){
                        for(int yb = y - 1; yb < y + 2; yb++){
                            if(xb < 0 || yb < 0 || yb >= intersactionImg.height() || xb >= intersactionImg.width()) 
                            	continue;
                            if(p2 == (int)round(yb * sin(theta2)) + (int)round(xb * cos(theta2))) {
                                intersactionImg.draw_circle(x, y, 5, yellow);
                                HoughPos tmp(x, y);
                                result.push_back(tmp);
                                flag = true;
                                break;
                            }
                        }
                        if(flag) 
                        	break;
                    }
                }
            }
        }
    }
}