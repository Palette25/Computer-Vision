/*
* Preprocess.h -- Perform preprocessing before number segmentation
*/
#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "utils.h"

class PreProcess {
public:
	PreProcess(CImg<unsigned char>& input, vector<vector<square> >& square, int seq);
	CImg<unsigned char> resizeImg(CImg<unsigned char>& input);
	void printStore(int seq);

	// Getter methods
	string getSubDir();

private:
	string subDir;
	vector<vector<CImg<unsigned char> > > imgVec;

};

#endif