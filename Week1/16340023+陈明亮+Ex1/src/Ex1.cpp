#include "CImg.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace cimg_library;
using namespace std;

/*
* CImgProcessor is the class for Exercise One
*/
class CImgProcessor{
public:
    // Input the image path, load the target image
    CImgProcessor(string path_){
        img.load(path_.c_str());
    }

    void display(){
        img.display();
    }

    // Change white area into red, black area into green
    void changeColor(){
        cimg_forXY(img, x, y){
            // White color's RGB is (255, 255, 255), Red's RGB is(255, 0, 0)
            if(img(x, y, 0) >= 200 && img(x, y, 1) >= 200 && img(x, y, 2) >= 200){
                img(x, y, 0) = 255;
                img(x, y, 1) = 0;
                img(x, y, 2) = 0;           
            }
            // Black color's RGB is (0, 0, 0), Green's RGB is(0, 255, 0)
            if(!img(x, y, 0) && !img(x, y, 1) && !img(x, y, 2)){
                img(x, y, 1) = 255;
            }
        }
    }

    void drawBlueCircle(pair<int, int> center, int radius){
        cimg_forXY(img, x, y){
            if(getDistance(center, make_pair(x, y)) <= radius){
                img(x, y, 0) = img(x, y, 1) = 0;
                img(x, y, 2) = 255;
            }
        }
    }

    void drawYellowCircle(pair<int, int> center, int radius){
        cimg_forXY(img, x, y){
            if(getDistance(center, make_pair(x, y)) <= radius){
                img(x, y, 0) = img(x, y, 1) = 255;
                img(x, y, 2) = 0;
            }
        }
    }

    void drawBlueLine(pair<int, int> center, double angle, int length){
        for(int i=1; i<=100; i++){
            double x_ = center.first + i * cos(angle/180 * cimg::PI),
                   y_ = center.second + i * sin(angle/180 * cimg::PI);
            // Tansform double into int, with round-up(四舍五入)
            int x = x_ + 0.5, y = y_ + 0.5;
            img(x, y, 0) = img(x, y, 1) = 0;
            img(x, y, 2) = 255;
        }
    }

    void storeImg(string path_){
        img.save(path_.c_str());
    }


private:
    CImg<unsigned char> img;
    inline int getDistance(pair<int, int> x, pair<int, int> y){
        return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
    }
};

int main(){
    CImgProcessor temp("1.bmp");
    temp.changeColor();
    temp.drawBlueCircle(make_pair(50, 50), 30);
    temp.drawYellowCircle(make_pair(50, 50), 3);
    temp.drawBlueLine(make_pair(0, 0), 35, 100);
    temp.storeImg("2.bmp");
    temp.display();
}