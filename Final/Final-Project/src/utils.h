#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <set>
#include <vector>
#include <stack>
#include <queue>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <windows.h>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

#define ANGLE 15
#define CHANNEL_NUM 3
#define BOUNDER 8

// Structure for storing hough point position
struct HoughPos {
    int x;
    int y;
    int val;
    HoughPos(int _x, int _y, int _val):x(_x),y(_y),val(_val) {}
    HoughPos(int _x, int _y):x(_x),y(_y) {}
    HoughPos(){}
};

// Hough Transform line
struct line {
    double k;
    double b;
    line(double _k, double _b):k(_k),b(_b) {}
};

// Basic RGB Pixel structure
struct Pixel {
    unsigned char val[CHANNEL_NUM];
    Pixel(){}
    Pixel(unsigned char _r, unsigned char _g, unsigned char _b) {
        val[0] = _r;
        val[1] = _g;
        val[2] = _b;
    }
    Pixel(unsigned char _g) {
        val[0] = _g;
    }
};

// Segmentation Position
struct position {
    int x;
    int y;
    position(){
        x = 0;
        y = 0;
    }
    position(int _x, int _y):x(_x), y(_y) {}
    void getRandPos(int width, int height) {
        x = rand() % width;
        y = rand() % height;
    }
    void setPosition(int _x, int _y) {
        x = _x;
        y = _y;
    }
};

// Homography matrix
struct Homography {
    double H[3][3];
    Homography(double w11, double w12, double w13, double w21, 
            double w22, double w23, double w31, double w32) {
        H[0][0] = w11;
        H[0][1] = w12;
        H[0][2] = w13;
        H[1][0] = w21;
        H[1][1] = w22;
        H[1][2] = w23;
        H[2][0] = w31;
        H[2][1] = w32;
        H[2][2] = 1;
    }
    Homography(){}
};

// Point pair for projection warping
struct pointPair {
    int pos1[2];
    int pos2[2];
    double distance;
    pointPair() {}
    void swap() {
        int tmp[2];
        tmp[0] = pos1[0];
        tmp[1] = pos1[1];
        pos1[0] = pos2[0];
        pos1[1] = pos2[1];
        pos2[0] = tmp[0];
        pos2[1] = tmp[1];
    }
};

// Square for Partition area selection
struct square {
    HoughPos lt;
    HoughPos lb;
    HoughPos rt;
    HoughPos rb;
    square(HoughPos lt_, HoughPos lb_, HoughPos rt_, HoughPos rb_) {
        lt = lt_;
        lb = lb_;
        rt = rt_;
        rb = rb_;
    }
};

static bool sorting(pointPair &a, pointPair &b) {
    return (a.distance < b.distance);
}

#endif