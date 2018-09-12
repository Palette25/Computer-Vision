#include "CImg.h"
#include <iostream>
#include <cmath>

using namespace cimg_library;
using namespace std;

int main(){
    // Step 1
    CImg<unsigned char> img("1.bmp");
    // Step 2
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
    // Step 3
    unsigned char blue[] = {0, 0, 255};
    img.draw_circle(50, 50, 30, blue);
    // Step 4
    unsigned char yellow[] = {255, 255, 0};
    img.draw_circle(50, 50, 3, yellow);
    // Step 5
    int x_ = cos((double)7/36 * cimg::PI)*100 + 0.5, y_ = sin((double)7/36 * cimg::PI)*100 + 0.5;
    img.draw_line(0, 0, x_, y_, blue);
    // Step 6
    img.save("2.bmp");
    // Display
    img.display();
    return 0;
}