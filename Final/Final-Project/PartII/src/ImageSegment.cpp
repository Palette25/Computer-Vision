#include "ImageSegment.h"

ImageSegmenter::ImageSegmenter(string path, float sigma){
	CImg<unsigned char> src(path.c_str());
    this->grayImg = toGrayScale(src);
    this->grayImg = grayImg.get_blur(sigma);
    performKmeans(grayImg);
}

ImageSegmenter::ImageSegmenter(CImg<unsigned char>& input){
	performKmeans(grayImg);
}

void ImageSegmenter::performKmeans(CImg<unsigned char>& input){
	core1.setPosition(input.width() / 2, input.height() / 2);
    core2.setPosition(input.width() - 1, input.height() - 1);
    vector<position> set1;
    vector<position> set2;
    // Dead loop to detect k-means clusters
    while (true){
        set1.clear();
        set2.clear();
        // Get two clusters (0 and 255)
        cimg_forXY(input, x, y){
        	int distance1 = abs((int)input(x, y) - (int)input(core1.x, core1.y));
            int distance2 = abs((int)input(x, y) - (int)input(core2.x, core2.y));
            // Judge point belongs to which cluster
            position tmp(x, y);
            if (distance1 <= distance2){
                set1.push_back(tmp);
            }
            else{
                set2.push_back(tmp);
            }
        }
        position currentCore1 = getMeans(set1, input);
        position currentCore2 = getMeans(set2, input);
        // Judge equal
        if (isEqual(currentCore1, core1) && isEqual(currentCore2, core2))
            break;
        core1.setPosition(currentCore1.x, currentCore1.y);
        core2.setPosition(currentCore2.x, currentCore2.y);
    }
    CImg<unsigned char> tmp = CImg<unsigned char>(input.width(), input.height(), 1, 1, 0);
    this->result = tmp;
    for (int i = 0; i < set1.size(); i++){
        tmp(set1[i].x, set1[i].y) = 255;
    }
    tmp = generate(tmp);
    this->block = tmp;
    // Painting points decided by their cluster
    cimg_forXY(tmp, x, y){
        if (tmp(x, y) == 0)
            continue;
        bool flag = false;
        for(int i = x - 2; i < x + 3; i++){
            for(int j = y - 2; j < y + 3; j++){
                if(i < 0 || j < 0 || i >= tmp.width() || j >= tmp.height()) continue;
                if(tmp(i, j) == 0) {
                    flag = true;
                    break;
                }
            }
            if(flag) break;
        }
        if(flag) {
        	this->result(x, y) = 255;
        }
    }
}

CImg<unsigned char> ImageSegmenter::generate(CImg<unsigned char>& input){
	HoughPos currentPos(input.width() / 2, input.height() / 2);
    this->posStack.push(currentPos);
    // Init Generation State variables
    for (int i = 0; i < input.width(); i++){
        vector<bool> column;
        for (int j = 0; j < input.height(); j++){
            bool flag = false;
            column.push_back(flag);
        }
        this->visited.push_back(column);
    }

    CImg<unsigned char> generation(input.width(), input.height(), 1, 1, 0);
    // Avoid noise
    cimg_forY(generation, y){
        int count = 0;
        cimg_forX(generation, x){
            if(input(x, y) == 255) count++;
        }
        if(count < 20){
            cimg_forX(generation, x){
                input(x, y) = 0;
            }
        }

    }
    // Perform DFS to generate new state
    while (!this->posStack.empty()){
        HoughPos currentPos = this->posStack.top();
        this->posStack.pop();
        if (this->visited[currentPos.x][currentPos.y])
            continue;
        this->visited[currentPos.x][currentPos.y] = true;
        generation(currentPos.x, currentPos.y) = 255;
        // Eight Neighbor Search
        for (int i = currentPos.x - 1; i < currentPos.x + 2; i++){
            for (int j = currentPos.y - 1; j < currentPos.y + 2; j++){
                if (i >= 0 && i < input.width() && j >= 0 && j < input.height()){
                    if (i == currentPos.x && j == currentPos.y)
                        continue;
                    if (input(i, j) == 255){
                        HoughPos nextPos(i, j);
                        this->posStack.push(nextPos);
                    }
                }
            }
        }
    }
    return generation;
}

position ImageSegmenter::getMeans(vector<position>& set, CImg<unsigned char>& src){
	float means = 0;
    // Get mean of RGB channels
    for (int i = 0; i < set.size(); i++){
        means += (float)src(set[i].x, set[i].y);
    }

    means /= (float)set.size();
    float minDistance = 10000;
    position result;
    // Calculate means
    for (int i = 0; i < set.size(); ++i){
        float distance = abs((float)src(set[i].x, set[i].y, 0) - means);
        if (distance < minDistance){
            result.setPosition(set[i].x, set[i].y);
            minDistance = distance;
        }
    }
    return result;
}

CImg<unsigned char> ImageSegmenter::toGrayScale(CImg<unsigned char>& input){
	CImg<unsigned char> gray = CImg<unsigned char>(input.width(), input.height(), 1, 1, 0); //To one channel
    cimg_forXY(input, x, y){
        gray(x, y) = input(x, y, 2) * 0.2126 + input(x, y, 1) * 0.7152 + input(x, y, 0) * 0.0722;
    }
    return gray;
}

bool ImageSegmenter::isEqual(position& a, position& b){
	return (a.x == b.x && a.y == b.y);
}

CImg<unsigned char> ImageSegmenter::getOutputImg(){
	return this->result;
}

CImg<unsigned char> ImageSegmenter::getBlockImg(){
	return this->block;
}