/*
* Program: main.cpp
* Usage: Read target file-dir's all images and stitch them
*/
#include <io.h>
#include <cstring>
#include "ImageStitching.h"
#include "Blending.h"
#include "Matching.h"
#include "Equalizing.h"

vector<string> getTargetImages(string dirPath);

int main(int argc, char* argv[]){
	string dirPath;
	if(argc < 2){
		cout << "Please enter the images dir path: ";
		cin >> dirPath;
	}else {
		dirPath = argv[1];
	}
	// Scan images files paths
	vector<string> imagesPath = getTargetImages(dirPath);
	// Judge dirPath validation
	if(imagesPath.size() == 0){
		cout << "Traget dir contains no images!\nExit." << endl;
		return 0;
	}
	cout << "Begin Stitching" << endl;
	// Begin stitching
	vector<CImg<unsigned char>> inputImages;
	for(string ele : imagesPath){
		inputImages.push_back(CImg<unsigned char>(ele.c_str()));
	}
	Stitcher st(inputImages);
	CImg<unsigned char> stitchedResult = st.stitchImages();
	cout << "End Stitching" << endl;
	// Final equalization
	cout << "Start Equalizing" << endl;
	Equalizater ez(stitchedResult);
	CImg<unsigned char> result = ez.Equalization();
	cout << "End Equalizing" << endl;

	result.display("Final Image"); 
	result.save("output/result.bmp");

	return 0;
}

// Read all images from dir
vector<string> getTargetImages(string dirPath){
	vector<string> result;
	long file = 0;  string path = "";
	// Define file infos
	struct _finddata_t fileInfo;
	if((file = _findfirst(path.assign(dirPath).append("\\*").c_str(), &fileInfo)) != -1){
		// If can find next, keep looping to push paths into result
		while(_findnext(file, &fileInfo) == 0){
			if((fileInfo.attrib & _A_SUBDIR)){
				if(strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0){
					result.push_back(path.assign(dirPath).append("\\").append(fileInfo.name));
				}
			}else {
				result.push_back(path.assign(dirPath).append("\\").append(fileInfo.name));
			}
		}
		_findclose(file);
	}
	return result;
}