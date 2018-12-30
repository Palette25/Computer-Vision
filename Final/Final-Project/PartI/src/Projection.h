/*
* Projection.h -- For projection Interpolcation
*/
#ifndef PROJECTION_H
#define PROJECTION_H

#include "utils.h"

static unsigned char bilinearInterpolation(const CImg<unsigned char> &src, double x, double y, int channel)
{
    int x_floor = floor(x), y_floor = floor(y);
    int x_ceil = ceil(x) >= (src.width() - 1) ? (src.width() - 1) : ceil(x);
    int y_ceil = ceil(y) >= (src.height() - 1) ? (src.height() - 1) : ceil(y);
    x_floor = x_floor < 0? 0 : x_floor;
    y_floor = y_floor < 0? 0 : y_floor;
    x_ceil >= src.width()? (src.width() - 1) : x_ceil;
    y_ceil >= src.height()? (src.height() - 1) : y_ceil;
    
    double a = x - x_floor, b = y - y_floor;

    Pixel leftdown = Pixel(src(x_floor, y_floor, 0), src(x_floor, y_floor, 1), src(x_floor, y_floor, 2));
    Pixel lefttop = Pixel(src(x_floor, y_ceil, 0), src(x_floor, y_ceil, 1), src(x_floor, y_ceil, 2));
    Pixel rightdown = Pixel(src(x_ceil, y_floor, 0), src(x_ceil, y_floor, 1), src(x_ceil, y_floor, 2));
    Pixel righttop = Pixel(src(x_ceil, y_ceil, 0), src(x_ceil, y_ceil, 1), src(x_ceil, y_ceil, 2));
    return (unsigned char)((1.0 - a) * (1.0 - b) * (double)leftdown.val[channel] + a * (1.0 - b) * (double)rightdown.val[channel] +
            a * b * (double)righttop.val[channel] + (1.0 - a) * b * (double)lefttop.val[channel]);
}

static unsigned char singleBilinearInterpolation(const CImg<unsigned char> &src, double x, double y, int channel)
{
    int x_floor = floor(x), y_floor = floor(y);
    int x_ceil = ceil(x) >= (src.width() - 1) ? (src.width() - 1) : ceil(x);
    int y_ceil = ceil(y) >= (src.height() - 1) ? (src.height() - 1) : ceil(y);
    x_floor = x_floor < 0? 0 : x_floor;
    y_floor = y_floor < 0? 0 : y_floor;
    x_ceil >= src.width()? (src.width() - 1) : x_ceil;
    y_ceil >= src.height()? (src.height() - 1) : y_ceil;

    double a = x - x_floor, b = y - y_floor;

    Pixel leftdown = Pixel(src(x_floor, y_floor, 0));
    Pixel lefttop = Pixel(src(x_floor, y_ceil, 0));
    Pixel rightdown = Pixel(src(x_ceil, y_floor, 0));
    Pixel righttop = Pixel(src(x_ceil, y_ceil, 0));
    return (unsigned char)((1.0 - a) * (1.0 - b) * (double)leftdown.val[channel] + a * (1.0 - b) * (double)rightdown.val[channel] +
                           a * b * (double)righttop.val[channel] + (1.0 - a) * b * (double)lefttop.val[channel]);
}

static bool isLegal(int x, int y, int srcWidth, int srcHeight, double theta,
                             double &srcX, double &srcY){
    srcX = (double)x * cos(theta) - (double)y * sin(theta);
    srcY = (double)x * sin(theta) + (double)y * cos(theta);
    if (srcX >= (0 - srcWidth / 2 - 1) && srcX <= srcWidth / 2 + 1 
        && srcY >= (0 - srcHeight / 2 - 1) && srcY <= srcHeight / 2 + 1){
        srcX += srcWidth / 2;
        srcY += srcHeight / 2;
        return true;
    }
    else{
        return false;
    }
}


static void bilinear(CImg<unsigned char> &src, CImg<unsigned char> &output, double theta){
    cimg_forXY(output, x, y){
        double src_x, src_y;
        
        if (isLegal(x - output.width() / 2, y - output.height() / 2, src.width(), src.height(), theta, src_x, src_y)){
            src_x = (src_x >= (double)src.width()) ? ((double)src.width() - 1.0) : (src_x < 0.0) ? 0.0 : src_x;
            src_y = (src_y >= (double)src.height()) ? ((double)src.height() - 1.0) : (src_y < 0.0) ? 0.0 : src_y;
            
            output(x, y) = singleBilinearInterpolation(src, src_x, src_y, 0);
        }
    }
}

static CImg<unsigned char> reotate_biliinar(CImg<unsigned char> &src, double theta){
    HoughPos lt(0 - src.width() / 2, 0 + src.height() / 2), lb(0 - src.width() / 2, 0 - src.height() / 2),
        rt(0 + src.width() / 2, 0 + src.height() / 2), rb(0 + src.width() / 2, 0 - src.height() / 2);

    HoughPos new_lt((int)(lt.x * cos(theta) + lt.y * sin(theta)), (int)(lt.y * cos(theta) - lt.x * sin(theta))),
        new_lb((int)(lb.x * cos(theta) + lb.y * sin(theta)), (int)(lb.y * cos(theta) - lb.x * sin(theta))),
        new_rt((int)(rt.x * cos(theta) + rt.y * sin(theta)), (int)(rt.y * cos(theta) - rt.x * sin(theta))),
        new_rb((int)(rb.x * cos(theta) + rb.y * sin(theta)), (int)(rb.y * cos(theta) - rb.x * sin(theta)));

    int width = abs(new_rt.x - new_lb.x) > abs(new_lt.x - new_rb.x) ? abs(new_rt.x - new_lb.x) : abs(new_lt.x - new_rb.x);
    int height = abs(new_lt.y - new_rb.y) > abs(new_lb.y - new_rt.y) ? abs(new_lt.y - new_rb.y) : abs(new_lb.y - new_rt.y);
    CImg<unsigned char> output(width, height, 1, 1, 255);
    bilinear(src, output, theta);

    cimg_forXY(output, x, y){
        if (output(x, y) > 200)
            output(x, y) = 255;
        else
            output(x, y) = 0;
    }
    return output;
}

#endif