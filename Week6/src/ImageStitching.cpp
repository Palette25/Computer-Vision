#include "ImageStitching.h"

Stitcher::Stitcher(vector<CImg<unsigned char>> input){
	for(CImg<unsigned char> ele : input){
		this->src.push_back(ele);
	}
	this->wr = Warper();
	this->mt = Matcher();
	this->ut = Utils();
}

// Start-up of image stitching, matched input images and stitch them
CImg<unsigned char> Stitcher::stitchImages(){
	// Fill features
	for(int i=0; i<src.size(); i++){
		cout << "Preprocess input image " << i << endl;
		// Warping iamge into spherical coordinates
		cout << "Spherical projection started." << endl;
		src[i] = ut.cylinderizeImg(src[i]);
		cout << "Spherical projection ended." << endl;
		// Turn into gray and perform SIFT
		CImg<unsigned char> temp = ut.toGrayImage(src[i]);
		SIFT sift(temp);
		cout << "Extracting SIFT feature started." << endl;
		this->features.push_back(sift.getSiftFeatures());
		cout << "Extracting SIFT feature ended." << endl;
		cout << "Preprocess input image " << i << " ended." << endl;
	}
	// Using match methods to align neighbor images
	vector<vector<int>> matchIndex(src.size());
	bool stitchFlag[20][20] = {false};
	cout << "Searching adjacent images..." << endl;
	for(int i=0; i<src.size(); i++){
		for(int j=0; j<src.size(); j++){
			if(i == j) continue;
			// Extract feature points pair from two images
			vector<keyPointPair> pairs = mt.scanPointPairs(features[i], features[j]);
			// Neighbor judge
			if(pairs.size() >= 20){
				stitchFlag[i][j] = true;
				cout << "Image " << i << " and Image " << j << " are adjacent." << endl;
				matchIndex[i].push_back(j);
			}
		}
	}
	// Using RANSAC to stitch
	int startIndex = computeMiddle(matchIndex);
	int prevIndex = startIndex;
	CImg<unsigned char> curr = src[startIndex];
	// Make queue to store unstitched indexes
	queue<int> store;  store.push(startIndex);
	cout << "Stitching begin." << endl;
	while(!store.empty()){
		int temp = store.front();
		store.pop();
		// Judge whether need to stitch
		for(int i=matchIndex[temp].size()-1; i>=0; i--){
			int dstIndex = matchIndex[temp][i];
			if(stitchFlag[temp][dstIndex] == false){
				continue;
			}else {
				stitchFlag[temp][dstIndex] = false;
				stitchFlag[dstIndex][temp] = false;
				store.push(dstIndex);
			}
			// Matching
			vector<keyPointPair> srcTodst = mt.scanPointPairs(features[temp], features[dstIndex]);
			vector<keyPointPair> dstTosrc = mt.scanPointPairs(features[dstIndex], features[temp]);
			if(srcTodst.size() > dstTosrc.size()){
				dstTosrc.clear();
				for(int j=0; j<srcTodst.size(); j++){
					dstTosrc.push_back(keyPointPair(srcTodst[j].p2, srcTodst[j].p1));
				}
			}else {
				srcTodst.clear();
				for(int j=0; j<dstTosrc.size(); j++){
					srcTodst.push_back(keyPointPair(dstTosrc[j].p2, dstTosrc[j].p1));
				}
			}
			// Homography and perform RANSAC
			Axis forward = mt.RANSAC(dstTosrc);
			Axis backward = mt.RANSAC(srcTodst);

			float min_x = wr.getMinXAfterWarping(src[dstIndex], forward);
			min_x = (min_x < 0) ? min_x : 0;
			float min_y = wr.getMinYAfterWarping(src[dstIndex], forward);
			min_y = (min_y < 0) ? min_y : 0;
			float max_x = wr.getMaxXAfterWarping(src[dstIndex], forward);
			max_x = (max_x >= curr.width()) ? max_x : curr.width();
			float max_y = wr.getMaxYAfterWarping(src[dstIndex], forward);
			max_y = (max_y >= curr.height()) ? max_y : curr.height();

			int new_width = ceil(max_x - min_x);
			int new_height = ceil(max_y - min_y);

			CImg<unsigned char> temp1(new_width, new_height, 1, src[dstIndex].spectrum(), 0);
			CImg<unsigned char> temp2(new_width, new_height, 1, src[dstIndex].spectrum(), 0);

			// Warp destination image by homography
			wr.warpImage(src[dstIndex], temp1, backward, min_x, min_y);
			// Move stitched image offset
			wr.moveImageByOffset(curr, temp2, min_x, min_y);
			// Mark features coordinates by homography
			addFeaturesByHomoegraphy(features[dstIndex], forward, min_x, min_y);
			// Mark features pairs by offset
			adjustOffset(features[prevIndex], min_x, min_y);

			// Blend result matched images
			cout << "Begin blend image " << temp << " and image " << dstIndex << endl;
			Blender blender(temp1, temp2);
			curr = blender.blendImages();
			prevIndex = dstIndex;
			cout << "Image " << temp <<  " and Image " << dstIndex << " has stitched." << endl;
		}
	}
	return curr;
}

int Stitcher::computeMiddle(vector<vector<int>>& indexes){
	int one_side = 0;
	for (int i = 0; i < indexes.size(); i++) {
		if (indexes[i].size() == 1) {
			one_side = i;
			break;
		}
	}
	int middle_index = one_side;
	int pre_middle_index = -1;
	int n = indexes.size() / 2;
	while (n--) {
		for (int i = 0; i < indexes[middle_index].size(); i++) {
			if (indexes[middle_index][i] != pre_middle_index) {
				pre_middle_index = middle_index;
				middle_index = indexes[middle_index][i];
				break;
			}
		}
	}
	return middle_index;
}

// Getting (x, y) by warping methods and add features
void Stitcher::addFeaturesByHomoegraphy(map<vector<float>, VlSiftKeypoint>& feature, Axis H, float offset_x, float offset_y){
	for (auto iter = feature.begin(); iter != feature.end(); iter++) {
		float cur_x = iter->second.x;
		float cur_y = iter->second.y;
		iter->second.x = wr.getXAfterWarping(cur_x, cur_y, H) - offset_x;
		iter->second.y = wr.getYAfterWarping(cur_x, cur_y, H) - offset_y;
		iter->second.ix = int(iter->second.x);
		iter->second.iy = int(iter->second.y);
	}
}

// Adjust offsets
void Stitcher::adjustOffset(map<vector<float>, VlSiftKeypoint>& feature, int offset_x, int offset_y){
	for (auto iter = feature.begin(); iter != feature.end(); iter++) {
		iter->second.x -= offset_x;
		iter->second.y -= offset_y;
		iter->second.ix = int(iter->second.x);
		iter->second.iy = int(iter->second.y);
	}
}