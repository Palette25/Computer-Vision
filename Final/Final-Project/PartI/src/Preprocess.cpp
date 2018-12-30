#include "Preprocess.h"

PreProcess::PreProcess(CImg<unsigned char>& input, vector<vector<square> >& square, int seq){
	// View Images
	for(int i = 0; i < square.size(); i++){
        vector<CImg<unsigned char> > imgTmp;
        for(int j = 0; j < square[i].size(); j++){
            CImg<unsigned char> part(square[i][j].rt.x - square[i][j].lt.x + 1, square[i][j].lb.y - square[i][j].lt.y + 1, 1, 1, 0);
            for(int x = square[i][j].lt.x; x <= square[i][j].rt.x; x++){
                for(int y = square[i][j].lt.y; y <= square[i][j].lb.y; y++){
                    part(x - square[i][j].lt.x, y - square[i][j].lt.y) = input(x, y);
                }
            }
            CImg<unsigned char> gray = part;
            gray = resizeImg(gray);
            gray = gray.resize(28, 28, true);
            imgTmp.push_back(gray);
        }
        this->imgVec.push_back(imgTmp);
    }
    // Perform Image reading
    string storePath = "../temp/";
    // Build sub dir
    this->subDir = storePath + "img";
    this->subDir += to_string(seq);
    // Make dir
    if(access(subDir.c_str(), 0) == -1){
    	mkdir(subDir.c_str());
    }
    printStore(seq);
}

// Resizing result images and perform correcting
CImg<unsigned char> PreProcess::resizeImg(CImg<unsigned char>& input){
	// Step 1. Image Centering
	bool flag = false;
	int firstX = 0, firstY = 0;
    int lastX = 0, lastY = 0;
    // First X
    cimg_forX(input, x){
        cimg_forY(input, y){
            if(input(x ,y) != 255){
                firstX = x;
                flag = true;
                break;
            }
        }
        if(flag) break;
    }
    // First Y
    flag = false;
    cimg_forY(input, y){
        cimg_forX(input, x){
            if(input(x ,y) != 255){
                firstY = y;
                flag = true;
                break;
            }
        }
        if(flag) break;

    }
    // Last X
    flag = false;
    for(int x = input.width() - 1; x >= 0; x--){
        for(int y = 0; y < input.height(); y++){
            if(input(x, y) != 255){
                lastX = x;
                flag = true;
                break;
            }
        }
        if(flag) break;
    }
    // Last Y
    flag = false;
    for(int y = input.height() - 1; y >= 0; y--){
        for(int x = 0; x < input.width(); x++){
            if(input(x, y) != 255){
                lastY = y;
                flag = true;
                break;
            }
        }
        if(flag) break;
    }
	// Step 2. Image Resizing
	int xLen = lastX - firstX + 1;
    int yLen = lastY - firstY + 1;
    CImg<unsigned char> tmp(xLen + 8, yLen + 8, 1, 1, 255);
    for(int i = 4, srcX = firstX; srcX <= lastX; i++, srcX++){
        for(int j = 4, srcY = firstY; srcY <= lastY; j++, srcY++){
            tmp(i, j) = input(srcX, srcY);
        }
    }
    return tmp;
}

// Store image and provide result segmentation txt
void PreProcess::printStore(int seq){
    string path = subDir + "/result.txt";
    ofstream out(path.c_str());
    for(int i = 0; i < imgVec.size(); i++){
        for(int j = 0; j < imgVec[i].size(); j++){
        	// Store cutting images
            string finalPath = subDir + "/" + to_string(i) + "_" + to_string(j) + ".bmp";
            imgVec[i][j].save(finalPath.c_str());
        }
        out << to_string(imgVec[i].size())<<endl;
    }
    out.close();

}

// Getter methods
string PreProcess::getSubDir(){
	return this->subDir;
}