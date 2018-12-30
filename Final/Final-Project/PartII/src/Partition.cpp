#include "Partition.h"

Partition::Partition(CImg<unsigned char>& input, int seq){
	this->seq = seq;
    this->grayImg = ImageSegmenter::toGrayScale(input);
    CImg<unsigned char> grayScale = this->grayImg;
    // Single Threshold Detect
    
    this->grayImg = threshold(grayScale, 15, 0.08);
    CImg<unsigned char> dilationed = this->grayImg;
    cimg_forXY(dilationed, x, y){
        if(dilationed(x, y) == 0) dilationed(x, y) = 255;
        else dilationed(x, y) = 0;
    }
    this->grayImg = dilationed;
    
    this->grayImg = EdgeDetection(this->grayImg, 6);
    HoughTransformer hough(0, 0, dilationed, 0, false);
    
    cimg_forXY(dilationed, x, y){
        if(dilationed(x, y) == 0) dilationed(x, y) = 255;
        else dilationed(x, y) = 0;
        if(this->grayImg(x, y) == 0) this->grayImg(x, y) = 255;
        else this->grayImg(x, y) = 0;
    }
    // Randon rotate
    double theta = hough.theta;
    theta = (theta - 90.0);
    // Perform Partition
    double ror = theta / 180.0 * cimg::PI;
    this->grayImg = reotate_biliinar(this->grayImg, ror);
    findDividingLine(3, 4);
    divideColumn(4);
}

CImg<unsigned char> Partition::threshold(CImg<unsigned char>& input, int size, float thres){
	CImg<unsigned char> result(input.width(), input.height(), 1, 1, 255);
    CImg<int> integral(input.width(), input.height(), 1, 1, 0);
    cimg_forY(result, y){
        int sum = 0;
        cimg_forX(result, x){
            sum += input(x, y);
            if(y == 0){
                integral(x, y) = sum;
            }else{
                integral(x, y) = integral(x, y - 1) + sum;
            }
        }
    }

    // Self-Suitting Threshold
    cimg_forY(input, y) {
        int y1 = (y - size > 0) ?y - size : 0;
        int y2 = (y + size < input.height()) ? (y + size) : (input.height() - 1);
        cimg_forX(input, x) {
            int x1 = (x - size > 0) ? x - size : 0;
            int x2 = (x + size < input.width()) ? (x + size) : (input.width() - 1);
            int count = (x2 - x1) * (y2 - y1);
            int sum = integral(x2, y2) - integral(x1, y2) -
                            integral(x2, y1) + integral(x1, y1);
            if (input(x, y) * count < sum * (1.0 - thres)) {
                result(x, y) = 0;
            }
        }
    }
    
    CImg<bool> isVisited(input.width(), input.height(), 1, 1, false);
    cimg_forXY(isVisited, x, y){
        if(x > BOUNDER && x < (isVisited.width() - BOUNDER) && y > BOUNDER && y < (isVisited.height() - BOUNDER)) 
        	continue;
        if(isVisited(x, y) || result(x, y) != 0) 
        	continue;
        HoughPos currentPos(x, y);
        stack<HoughPos> posStack;
        posStack.push(currentPos);
        isVisited(x, y) = true;
        // DFS Searching
        while (!posStack.empty()){
            HoughPos currentPos = posStack.top();
            posStack.pop();
            if (isVisited(currentPos.x,currentPos.y))
                continue;
            isVisited(currentPos.x,currentPos.y) = true;
            // Eight Neighbor searching
            for (int i = currentPos.x - 1; i < currentPos.x + 2; i++){
                for (int j = currentPos.y - 1; j < currentPos.y + 2; j++){
                    if (i >= 0 && i < isVisited.width() && j >= 0 && j < isVisited.height()){
                        if (i == currentPos.x && j == currentPos.y)
                            continue;
                        if (result(i, j) == 0){
                            HoughPos nextPos(i, j);
                            posStack.push(nextPos);
                        }
                    }
                }
            }
        }
    }
    cimg_forXY(result, x, y){
        if(isVisited(x, y)){
            result(x, y) = 255;
        }
    }
    return result;
}

// Divide Image accroding to columns
void Partition::divideColumn(int thres){
    for(int i = 1; i < linePos.size(); i++) {
        int barHeight = linePos[i] - linePos[i - 1];
        CImg<unsigned char> barItemImg = CImg<unsigned char>(grayImg.width(), barHeight, 1, 1, 0);
        cimg_forXY(barItemImg, x, y) {
            barItemImg(x, y, 0) = grayImg(x, linePos[i - 1] + 1 + y, 0);
        }

        vector<square> squareTmp;
        vector<int> dividePosXset = getColumnLine(barItemImg, thres);
        if(dividePosXset.empty())
            continue;
        int everyCol = 0;
        for(int j = 1; j < dividePosXset.size(); j++){
            // Reduce noise points
            square squ(HoughPos(dividePosXset[j - 1], linePos[i - 1]), HoughPos(dividePosXset[j - 1], linePos[i]),
                       HoughPos(dividePosXset[j], linePos[i - 1]), HoughPos(dividePosXset[j], linePos[i]));
            int count = 0;
            for(int y = squ.lt.y; y<= squ.lb.y; y++){
                bool flag = false;
                for(int x = squ.lt.x; x <= squ.rt.x; x++){
                    if(grayImg(x, y) == 0){
                        flag = true;
                        break;
                    }
                }
                if(flag) count++;
            }
            if(count > 6){
                everyCol++;
                squareTmp.push_back(squ);
            }
        }
        if(everyCol > 4){
            this->squareVec.push_back(squareTmp);
            // Draw lines
            unsigned char lineColor[1] = {0};
            for (int j = 0; j < dividePosXset.size(); j++) {
                dividedImg.draw_line(dividePosXset[j], linePos[i - 1],
                                      dividePosXset[j], linePos[i], lineColor);
            }
        }

    }

}

void Partition::findDividingLine(int singleThreshold, int threshold){
    CImg<int> histogram(grayImg.width(), grayImg.height(), 1, 1, 255);
    dividedImg = grayImg;

    // Scanning for histogram
    cimg_forY(grayImg, y){
        int blackPixel = 0;
        cimg_forX(grayImg, x) {
            if (grayImg(x, y) == 0)
                blackPixel++;
        }
        blackPixel = blackPixel > singleThreshold ? blackPixel : 0;
        blackPixels.push_back(blackPixel);
    }
    
    // Loop for line finding
    cimg_forY(grayImg, y){
        if(blackPixels[y] != 0){
            int pointer = y + 1;
            int counter = 0;
            while(pointer < grayImg.height() && blackPixels[pointer] != 0){
                pointer++;
                counter += blackPixels[pointer];
            }
            counter /= (pointer - y);
            pointer--;
            // Delete noise
            if(pointer < grayImg.height() - 1 &&  blackPixels[pointer] != 0 && counter < threshold){
                int distance = pointer - y + 1;
                if(distance < threshold){
                    for(int i = y; i <= pointer; i++){
                        blackPixels[i] = 0;
                    }
                }
            }
        }
    }
    
    for(int y = 0; y < blackPixels.size(); y++) {
        if(y == 0) continue;

        if (blackPixels[y] == 0 && blackPixels[y - 1] != 0)
            linePos.push_back(y);

        else if (blackPixels[y] != 0 && blackPixels[y - 1] == 0)
            linePos.push_back(y - 1);
    }
    
    unsigned char lineColor[1] = {0};

    vector<int> tmpVec = linePos;
    linePos.clear();
    
    // Delete empty lines
    for(int index = 0; index < tmpVec.size() - 1; index++){
        int counter = 0;
        for(int i = tmpVec[index]; i <= tmpVec[index + 1]; i++){
            if(blackPixels[i] != 0) counter++;
        }
        if((double)counter / (double)(tmpVec[index + 1] - tmpVec[index]) < 0.3) {
            int median = (tmpVec[index + 1] + tmpVec[index]) / 2;
            linePos.push_back(median);
            index++;
        } else{
            linePos.push_back(tmpVec[index]);
        }
    }
    linePos.push_back(tmpVec[tmpVec.size() - 1]);
    tmpVec = linePos;
    linePos.clear();
    int distance = tmpVec[tmpVec.size() - 1] - tmpVec[0];
    distance /= tmpVec.size();
    
    // Close Distance line combined 
    for(int index = 0; index < tmpVec.size() - 1; index++){
        if(tmpVec[index + 1] - tmpVec[index] < (distance / 3)) {
            int median = (tmpVec[index + 1] + tmpVec[index]) / 2;
            linePos.push_back(median);
            index++;
        } else{
            linePos.push_back(tmpVec[index]);
        }
    }
    
    linePos.push_back(tmpVec[tmpVec.size() - 1]);
    if(linePos[0] > 2){
        linePos[0] -= 2;
    }

    if(linePos[linePos.size() - 1] + 2 < grayImg.height()){
        linePos[linePos.size() - 1] += 2;
    }
    
    for(int i = 0; i < linePos.size(); i++){
        dividedImg.draw_line(0, linePos[i], grayImg.width() - 1, linePos[i], lineColor);
    }
}

// Get every column lines
vector<int> Partition::getColumnLine(CImg<unsigned char>& input, int thres){
    vector<int> countVec;
    cimg_forX(input, x){
        int blackPixel = 0;
        cimg_forY(input, y) {
            if (input(x, y) == 0)
                blackPixel++;
        }

        bool flag = (blackPixel > thres) ? true : false;
        if(x - 1 >= 0 && countVec[x - 1] != 0){
            flag = true;
        }
        if(!flag) {
            blackPixel = 0;
        }
        countVec.push_back(blackPixel);
    }

    int counter = 0;
    for(int i = 0; i < countVec.size(); i++){
        if(countVec[i] != 0) counter++;
    }

    // Delete empty rows
    if(counter == 0){
        vector<int> InflectionPosXs;
        InflectionPosXs.clear();
        return InflectionPosXs;
    }
    // Get Inflection Points
    vector<int> InflectionPosXs = getColumnInflectionPoints(countVec);
    unsigned char lineColor[1] = {0};
    for(int i = 0; i < InflectionPosXs.size(); i++){
        input.draw_line(InflectionPosXs[i], 0, InflectionPosXs[i], input.width() - 1, lineColor);
    }
    
    return InflectionPosXs;
}

vector<int> Partition::getColumnInflectionPoints(vector<int>& vec){
    vector<int> resultInflectionPosXs, tempInflectionPosXs;
    // Look up inflection points
    for(int i = 0; i < vec.size(); i++){
        // White to Black inflection
        if (i > 0 && vec[i] != 0 && vec[i - 1] == 0)
            tempInflectionPosXs.push_back(i - 1);
        // Black to White inflection
        else if (i > 0 && vec[i] == 0 && vec[i - 1] != 0)
            tempInflectionPosXs.push_back(i);
    }

    // Combine vectora
    vector<int> tmpVec = tempInflectionPosXs;
    tempInflectionPosXs.clear();

    if(tmpVec[0] > 5) 
        tmpVec[0] -= 5;
    if(tmpVec[tmpVec.size() - 1] + 5 < grayImg.width()) 
        tmpVec[tmpVec.size() - 1] += 5;
    tempInflectionPosXs.push_back(tmpVec[0]);

    for(int index = 1; index < tmpVec.size() - 1; index++){
        int counter = 0;
        for(int i = tmpVec[index]; i <= tmpVec[index + 1]; i++){
            if(vec[i] != 0) counter++;
        }
        
        if(counter == 0) {
            int median = (tmpVec[index + 1] + tmpVec[index]) / 2;
            tempInflectionPosXs.push_back(median);
            index++;
        } else{
            tempInflectionPosXs.push_back(tmpVec[index]);
        }
    }

    tempInflectionPosXs.push_back(tmpVec[tmpVec.size() - 1 ]);
    if(seq < 10){
        int max = 0;
        for(int i = 1; i < tempInflectionPosXs.size(); i++){
            if(tempInflectionPosXs[i] - tempInflectionPosXs[i - 1] > max) 
                max = tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
        }
        float distance = 0.0;
        for(int i = 1; i < tempInflectionPosXs.size(); i++){
            distance += tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
        }
        distance -= (float)max;

        distance /= (float)(tempInflectionPosXs.size() - 1);
        
        for(auto iter = tempInflectionPosXs.begin(); iter != tempInflectionPosXs.end(); iter++){
            if(iter != tempInflectionPosXs.begin() && (float)(*iter - *(iter - 1)) >= 2.077 * distance){
                int median = *iter - *(iter - 1);
                median /= 2;
                median += *(iter - 1);
                tempInflectionPosXs.insert(iter, median);
                iter = tempInflectionPosXs.begin();
            }
        }
    }
    return tempInflectionPosXs;
}

// Getter methods
vector<vector<square> > Partition::getSquare(){
	return this->squareVec;
}

CImg<unsigned char> Partition::getGrayImg(){
	return this->grayImg;
}

CImg<unsigned char> Partition::getDividedImg(){
	return this->dividedImg;
}