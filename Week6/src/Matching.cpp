#include "Matching.h"
// Scan two images' matched points and make pairs
vector<keyPointPair> Matcher::scanPointPairs(map<vector<float>, VlSiftKeypoint>& f1, map<vector<float>, VlSiftKeypoint> f2){
	// Use vlfeat's kdtree
	VlKDForest* forest = vl_kdforest_new(VL_TYPE_FLOAT, 128, 1, VlDistanceL1);

	float *data = new float[128 * f1.size()];
	int k = 0;
	for (auto it = f1.begin(); it != f1.end(); it++) {
		const vector<float> &descriptors = it->first;
		for (int i = 0; i < 128; i++) {
			data[i + 128 * k] = descriptors[i];
		}
		k++;
	}

	vl_kdforest_build(forest, f1.size(), data);

	vector<keyPointPair> res;

	VlKDForestSearcher* searcher = vl_kdforest_new_searcher(forest);
	VlKDForestNeighbor neighbours[2];

	for (auto it = f2.begin(); it != f2.end(); it++){
		float *temp_data = new float[128];

		for (int i = 0; i < 128; i++) {
			temp_data[i] = (it->first)[i];
		}
		int nvisited = vl_kdforestsearcher_query(searcher, neighbours, 2, temp_data);

		float ratio = neighbours[0].distance / neighbours[1].distance;
		if (ratio < 0.5) {
			vector<float> des(128);
			for (int j = 0; j < 128; j++) {
				des[j] = data[j + neighbours[0].index * 128];
			}

			VlSiftKeypoint left = f1.find(des)->second;
			VlSiftKeypoint right = it->second;
			res.push_back(keyPointPair(left, right));
		}

		delete[] temp_data;
		temp_data = NULL;
	}

	vl_kdforestsearcher_delete(searcher);
	vl_kdforest_delete(forest);

	delete[] data;
	data = NULL;

	return res;
}

// Image homography matching
Axis Matcher::Homography(vector<keyPointPair>& pair){
	CImg<float> A(4, 4, 1, 1, 0);
	CImg<float> b(1, 4, 1, 1, 0);

	for(int i=0; i<4; i++){
		A(0, i) = pair[i].p1.x;  A(1, i) = pair[i].p1.y;
		A(2, i) = pair[i].p1.x * pair[i].p1.y;
		A(3, i) = 1;
		b(0, i) = pair[i].p2.x;
	}

	CImg<float> temp1 = b.get_solve(A);

	for(int i=0; i<4; i++)
		b(0, i) = pair[i].p2.y;

	CImg<float> temp2 = b.get_solve(A);

	return Axis(temp1(0, 0), temp1(0, 1), temp1(0, 2), temp1(0, 3), temp2(0, 0), 
		temp2(0, 1), temp2(0, 2), temp2(0, 3));
}

// RANSAC Implementing
Axis Matcher::RANSAC(vector<keyPointPair>& pairs){
	srand(time(0));

	// Belive confidence, inline_ratio and make pair num to calculate turn number
	float conf = 0.99, inlineRate = 0.5;
	int pairNum = 4;
	int turns = ceil(log(1 - conf) / log(1 - pow(inlineRate, pairNum)));

	vector<int> max_inliner_indexs;

	while (turns--) {
		vector<keyPointPair> random_pairs;
		set<int> indexs;

		for (int i = 0; i < NUM_OF_PAIR; i++) {
			int index = random(0, pairs.size() - 1);
			while (indexs.find(index) != indexs.end()) {
				index = random(0, pairs.size() - 1);
			}
			indexs.insert(index);

			random_pairs.push_back(pairs[index]);
		}

		Axis H = Homography(random_pairs);
		
		vector<int> cur_inliner_indexs = getInlinerIndexs(pairs, H, indexs);
		if (cur_inliner_indexs.size() > max_inliner_indexs.size()) {
			max_inliner_indexs = cur_inliner_indexs;
		}
	}

	Axis t = leastSquares(pairs, max_inliner_indexs);

	return t;
}

Axis Matcher::leastSquares(vector<keyPointPair> pairs, vector<int> inliner_indexs){
	int calc_size = inliner_indexs.size();

	CImg<double> A(4, calc_size, 1, 1, 0);
	CImg<double> b(1, calc_size, 1, 1, 0);

	for (int i = 0; i < calc_size; i++) {
		int cur_index = inliner_indexs[i];

		A(0, i) = pairs[cur_index].p1.x;
		A(1, i) = pairs[cur_index].p1.y;
		A(2, i) = pairs[cur_index].p1.x * pairs[cur_index].p1.y;
		A(3, i) = 1;

		b(0, i) = pairs[cur_index].p2.x;
	}

	CImg<double> x1 = b.get_solve(A);

	for (int i = 0; i < calc_size; i++) {
		int cur_index = inliner_indexs[i];

		b(0, i) = pairs[cur_index].p2.y;
	}

	CImg<double> x2 = b.get_solve(A);

	return Axis(x1(0, 0), x1(0, 1), x1(0, 2), x1(0, 3), x2(0, 0), x2(0, 1), x2(0, 2), x2(0, 3));
}

vector<int> Matcher::getInlinerIndexs(vector<keyPointPair>& pairs, Axis H, set<int> indexes){
	vector<int> inliner_indexs;

	for (int i = 0; i < pairs.size(); i++) {
		if (indexes.find(i) != indexes.end()) {
			continue;
		}

		float real_x = pairs[i].p2.x;
		float real_y = pairs[i].p2.y;

		float x = wr.getXAfterWarping(pairs[i].p1.x, pairs[i].p1.y, H);
		float y = wr.getYAfterWarping(pairs[i].p1.x, pairs[i].p1.y, H);

		float distance = sqrt((x - real_x) * (x - real_x) + (y - real_y) * (y - real_y));
		if (distance < 4.0) {
			inliner_indexs.push_back(i);
		}
	}
	return inliner_indexs;
}