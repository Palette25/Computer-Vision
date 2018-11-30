/*
* Program: SIFT.cpp
* Usage: Implement SIFT class methods
*/
#include "SIFT.h"

SIFT::SIFT(CImg<float> src){
	this->srcImg = src;
}
// SIFT process steps
// Using up smapling and linear interpolation
CImg<float> SIFT::makeDoubleSizeLinear(CImg<float> input){
	int doubleWidth = input.width() * 2, doubleHeight = input.height() * 2;
	CImg<float> doubleImg(doubleWidth, doubleHeight, 1, 3, 1);
	cimg_forXY(doubleImg, x, y){
		doubleImg(x, y) = input(x / 2, y / 2);
	}
	// Use nearest neighbors to linear interpolcate
	// Upward and downward
	for(int i=0; i<doubleHeight; i+=2){
		for(int j=1; j<doubleWidth-1; j+=2)
			doubleImg(j, i) = 0.5 * (input(j/2, i/2) + input(j/2 + 1, i/2));
	}
	// Leftward and rightward
	for(int i=1; i<doubleHeight-1; i+=2){
		for(int j=0; j<doubleWidth; j+=2){
			doubleImg(j, i) = 0.5 * (input(j/2, i/2) + input(j/2, i/2+1));
		}
	}
	// Center
	for(int i=1; i<doubleHeight-1; i+=2){
		for(int j=1; j<doubleWidth-1; j+=2){
			doubleImg(j, i) = 0.25 * (input(j/2, i/2) + input(j/2+1, i/2) + input(j/2, i/2+1) + input(j/2+1, i/2+1));
		}
	}
	return doubleImg;
}
// 1. Preprocess input image
CImg<float> SIFT::InitImage(CImg<float> input){
	CImg<float> blur_img = input.get_blur(INITSIGMA);
	// Make bottom level of the pyrmid
	CImg<float> temp = makeDoubleSizeLinear(blur_img);
	// Pre blur
	double pre_sigma = 1.0;
	CImg<float> tt = temp.get_blur(pre_sigma);
	double sigma = sqrt((4 * INITSIGMA * INITSIGMA) + pre_sigma * pre_sigma);
	CImg<float> result = tt.get_blur(sigma);
	return result;
}

// 2. Establish gaussian octaves
ImageOctave* SIFT::BuildGaussianOctave(CImg<float> input){
	double temp = pow(2, 1.0f / ((float)SCALESPEROCTAVE));
	float pre_sigma, sigma, absolute_sigma;
	// Compute octaves number
	int dim = min(input.width(), input.height());
	this->octavesNum = min((int)(log((double)dim) / log(2.0)) - 2, 4);

	for(int i=0; i<this->octavesNum; i++){
		this->octaves.push_back(ImageOctave());
		this->DOGoctaves.push_back(ImageOctave());
		octaves[i].octave[0].level = input;
		// Init basic infos
		octaves[i].width = input.width();
		octaves[i].height = input.height();
		DOGoctaves[i].width = input.width();
		DOGoctaves[i].height = input.height();
		// Double-Based image size
		octaves[i].subSample = pow(2.0, i) * 0.5;
		if(i == 0){
			octaves[0].octave[0].sigma = sqrt(2.0);
			octaves[0].octave[0].absolute_sigma = sqrt(2.0);
		}else {
			octaves[i].octave[0].sigma = octaves[i-1].octave[SCALESPEROCTAVE].sigma;
			octaves[i].octave[0].absolute_sigma = octaves[i-1].octave[SCALESPEROCTAVE].absolute_sigma;
		}
		sigma = sqrt(2.0);
		for(int j=1; j<SCALESPEROCTAVE + 3; j++){
			int temp_sigma = sqrt(temp * temp-1) * sigma,
				length = (int)max(3.0f, (float)(2.0 * GAUSSKERN * temp_sigma + 1.0f));
			if(length % 2 == 0) length++;
			sigma = temp * sigma;
			absolute_sigma = sigma * (octaves[i].subSample);
			CImg<float> blurImg = octaves[i].octave[j-1].level.get_blur(temp_sigma);
			octaves[i].octave[j].sigma = sigma;
			octaves[i].octave[j].absolute_sigma = absolute_sigma;
			octaves[i].octave[j].length = length;
			octaves[i].octave[j].level = blurImg;
			CImg<float> res(input.width(), input.height(), 1, 3, 1);
			cimg_forXY(res, x, y){
				res(x, y) = octaves[i].octave[j].level(x, y) - octaves[i].octave[j-1].level(x, y);
			}
			DOGoctaves[i].octave[j-1].level = res;
		}
		input = makeHalfSize(octaves[i].octave[SCALESPEROCTAVE].level);
	}
}

// 3. Detect key points locations
int SIFT::DetectKeyPoints(){
	double cur_threshold = ((CURVATURE_THRESHOLD + 1) * (CURVATURE_THRESHOLD + 1)) / CURVATURE_THRESHOLD;
	for(int i=0; i<octavesNum; i++){
		for(int j=1; j<SCALESPEROCTAVE+1; j++){
			int dim = (int)(0.5 * (this->octaves[i].octave[j].length + 0.5));
			for(int n=dim; n<DOGoctaves[i].height - dim; n++){
				for(int m=dim; m<DOGoctaves[i].width - dim; m++){
					if(fabs((DOGoctaves[i].octave[j].level)(m ,n)) >= CONTRAST_THRESHOLD){
						if((DOGoctaves[i].octave[j].level)(m ,n) != 0.0){
							float downBound = (this->DOGoctaves[i].octave[j].level)(m ,n);
							// Judge whether satifies 26 middle maximal or minmal points
							bool flag1 = (downBound <= (this->DOGoctaves[i].octave[j].level)(m - 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m + 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m - 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m + 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m - 1, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j].level)(m + 1, n + 1)) &&    

								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n + 1)) && 

								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n - 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m, n + 1)) &&
								(downBound <= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n + 1));

							bool flag2 = (downBound <= (this->DOGoctaves[i].octave[j].level)(m, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m + 1, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m - 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m + 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m - 1, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j].level)(m + 1, n + 1)) &&    

								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m - 1, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j-1].level)(m + 1, n + 1)) && 

								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n - 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m - 1, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m, n + 1)) &&
								(downBound >= (this->DOGoctaves[i].octave[j+1].level)(m + 1, n + 1));

							if(flag1 || flag2){
								float Dxx = (this->DOGoctaves[i].octave[j].level)(m, n-1) + (this->DOGoctaves[i].octave[j].level)(m, n+1) - 2.0 * (this->DOGoctaves[i].octave[j].level)(m, n),
									  Dyy = (this->DOGoctaves[i].octave[j].level)(m-1, n) + (this->DOGoctaves[i].octave[j].level)(m+1, n) - 2.0 * (this->DOGoctaves[i].octave[j].level)(m, n),
									  Dxy = (this->DOGoctaves[i].octave[j].level)(m-1, n-1) + (this->DOGoctaves[i].octave[j].level)(m+1, n+1) - (this->DOGoctaves[i].octave[j].level)(m+1, n-1) - (this->DOGoctaves[i].octave[j].level)(m-1, n+1);
								float Tri_H = Dxx + Dyy, Det_H = Dxx * Dyy - Dxy * Dxy, ratio = (1.0 * Tri_H * Tri_H) / Det_H;
								if(Det_H >= 0.0 && ratio <= cur_threshold){
									this->keyPointsNum++;
									KeyPoint temp;
									temp.width = m * (this->octaves[i].subSample);
									temp.height = n * (this->octaves[i].subSample);
									temp.x  = m;  temp.y = n;
									temp.octave_order = i;  temp.level_order = j;
									temp.scale = (this->octaves[i].octave[j].absolute_sigma);
									this->keyPoint.push_back(temp);
								}
							}
						}
					}
				}
			}
		}
	}
	return this->keyPointsNum;
}

void SIFT::DisplayKeyPoints(){
	unsigned char color[3] = {255, 0, 0};
	for(KeyPoint temp : this->keyPoint){
		this->keyPointsImg.draw_circle(temp.width, temp.height, 1, color);
	}
}

// 4. Calculate gaussian image's gradients, magniture and direction
void SIFT::ComputeGrad_Mag_Dir(){
	for(int i=0; i<this->octavesNum; i++){
		for(int j=1; j<SCALESPEROCTAVE+1; j++){
			CImg<float> mag(this->octaves[i].width, this->octaves[i].height, 1, 1, 0);
			CImg<float> ori(this->octaves[i].width, this->octaves[i].height, 1, 1, 0);
			CImg<float> temp1(this->octaves[i].width, this->octaves[i].height, 1, 1, 0);
			CImg<float> temp2(this->octaves[i].width, this->octaves[i].height, 1, 1, 0);
			// Compute magniture
			for(int m=1; m<this->octaves[i].height-1; m++){
				for(int n=1; n<this->octaves[i].width-1; n++){
					// Direction-X
					temp1(n, m) = 0.5 * ((octaves[i].octave[j].level)(n+1, m) - (octaves[i].octave[j].level)(n-1, m));
					// Direction-Y
					temp2(n, m) = 0.5 * ((octaves[i].octave[j].level)(n, m+1) - (octaves[i].octave[j].level)(n, m-1));
					mag(n, m) = sqrt(temp1(n, m)*temp1(n, m) + temp2(n, m)*temp2(n, m));
					ori(n, m) = atan(temp2(n, m) / temp1(n, m));
					// Judge border
					if(ori(n, m) == PI)
						ori(n, m) = -PI;
				}
				this->magPyr.push_back(ImageOctave());  this->gradPyr.push_back(ImageOctave());
				this->magPyr[i].octave[j-1].level = mag;
				this->gradPyr[i].octave[j-1].level = ori;
			}
		}
	}
}

// Nearest neighbor binlinear filtering
int SIFT::FindNearestRotationNei(float angle, int count){
	// Comouted align bin
	angle += PI;  angle = angle / (PI * 2.0);  angle = angle * count;
	int temp = (int) angle;
	if(temp == count)
		return 0;
	else
		return temp;
}

// Average filtering
void SIFT::AverageWeakBins(float* bins, int count){
	for(int i=0; i<2; i++){
		float first = bins[0], last = bins[count - 1];
		for(int j=0; j<count; j++){
			float temp = bins[j],
				  nextOne = (j == count - 1) ? first : bins[(j+1) % count]; // Middle choosen
			bins[j] = (temp + nextOne + last) / 3.0;
			last = temp;
		}
	}
}

// Quadratic curve fitting
bool SIFT::InterpolateOrientation(float left, float middle, float right, float* correction, float* peak){
	float alpha = ((left + right) - 2.0 * middle) / 2.0;
	// Do fitting
	if(alpha == 0.0) return false;  // Failed
	float delta = (((left - middle) / alpha) - 1.0) / 2.0, beta = middle - delta * delta * alpha;
	if(delta < -0.5 || delta > 0.5)
		return false;
	*correction = delta;
	*peak = beta;
	return true;
}

void SIFT::AssignTheMainOrientation(){
	// set up the histogram of 36 bins
	int nums = 36;
	float step_num = 2.0 * PI / nums;
	float histOri[36];
	// Compute gradients
	for(int i=0; i<nums; i++)
		histOri[i] = -PI + i * step_num;
	float sigma = ((this->octaves[0].octave[SCALESPEROCTAVE].absolute_sigma)) / (this->octaves[0].subSample);
	int zero = (int)(max(3.0f, (float)(2 * GAUSSKERN * sigma + 1.0f)) * 0.5 + 0.5);
	// Assign oriention to all key points 
	for(KeyPoint ele : this->keyPoint){
		int x0 = ele.octave_order, x1 = ele.level_order, x2 = ele.y, x3 = ele.x;
		if((x2 >= zero) && (x2 < this->octaves[x0].height - zero) &&
		   (x3 >= zero) && (x3 < this->octaves[x0].width - zero)){
			float sigma1 = ((this->octaves[x0].octave[x1].absolute_sigma)) / (this->octaves[x0].subSample);
			// Make gaussian template
			CImg<float> temp = make2DGaussianKernel(sigma1);
			int dim = (int)(0.5 * temp.height());
			// Make hist
			float orientHist[36] = {0.0};
			int i=0, j=0;
			for(int x = x2 - dim; x<=(x2 + dim); x++, i++){
				for(int y = x3 - dim; y<=(x3 + dim); y++, j++){
					float dx = 0.5 * ((this->octaves[x0].octave[x1].level)(y+1, x) - (this->octaves[x0].octave[x1].level)(y-1, x));
					float dy = 0.5 * ((this->octaves[x0].octave[x1].level)(y, x+1) - (this->octaves[x0].octave[x1].level)(y, x-1));
					float mag = sqrt(dx * dx + dy * dy);
					float ori = atan(1.0f * dy / dx);
					int binIndex = FindNearestRotationNei(ori, 36);
					orientHist[binIndex] = orientHist[binIndex] + 1.0 * mag * temp(j, i);
				}
			}
			AverageWeakBins(orientHist, 36);
			// Find the max peak in gredient orientation
			float maxGrad = 0.0, maxPeak = 0.0, maxDegreeCorrect = 0.0;  
			int maxBin = 0;
			for(int k=0; k<36; k++){
				if(orientHist[k] > maxGrad){
					maxGrad = orientHist[k];
					maxBin = k;
				}
			}
			// Judge broken....
			int ttemp = maxBin == 0 ? 35 : maxBin - 1;
			bool judgeInterpolate = InterpolateOrientation(orientHist[ttemp], orientHist[maxBin], 
											orientHist[(maxBin + 1) % 36], &maxDegreeCorrect, &maxPeak);
			if(!judgeInterpolate)
				cout << "[Error] Parabola fitting broken!" << endl;
			// After getting peak value, then we can find key points orientations
			bool binIsKeyPoint[36];
			for(int i=0; i<36; i++){
				binIsKeyPoint[i] = false;
				if(i == maxBin){
					binIsKeyPoint[i] = true;
					continue;
				}
				if(orientHist[i] < 0.8 * maxPeak)
					continue;
				int left = (i == 0) ? 35 : i-1, right = (i+1) % 36;
				if(orientHist[i] <= orientHist[left] || orientHist[i] <= orientHist[right])
					continue;
				binIsKeyPoint[i] = true;
			}
			// Find other possible locations
			float road = (2.0 * PI) / 36;
			for(int i=0; i<36; i++){
				if(!binIsKeyPoint[i]) continue;
				int left1 = (i == 0) ? 35 : i-1, right1 = (i+1) % 36;
				float peak, corr, maxPeak1, maxCorr1;
				int ttmp1 = maxBin == 0 ? (36 - 1) : (maxBin - 1);
				bool judgeInterpolate1 = InterpolateOrientation(orientHist[ttmp1], orientHist[maxBin], 
											orientHist[(maxBin + 1) % 36], &corr, &peak);
				if(judgeInterpolate1 == false)
					cout << "[Error] Parabola fitting broken!" << endl;

				float degree = (i + corr) * road - PI;
				if(degree < -PI)
					degree += 2.0 * PI;
				else if(degree > PI)
					degree -= 2.0 * PI;
				// New the keyPoint descriptor
				KeyPoint newPoint;
				newPoint.width = ele.width;
				newPoint.height = ele.height;
				newPoint.x = ele.x;
				newPoint.y = ele.y;
				newPoint.octave_order = ele.octave_order;
				newPoint.level_order = ele.level_order;
				newPoint.scale = ele.scale;
				newPoint.ori = degree;
				newPoint.mag = peak;
				this->keyDescriptors.push_back(newPoint);
			}
		}
	}
}

void SIFT::DisplayOrientation(CImg<float> img){
	unsigned char red[] = {255, 0, 0};
	float init_scale = 3.0;
	for(KeyPoint ele : this->keyDescriptors){
		float scale = (this->octaves[ele.octave_order].octave[ele.level_order]).absolute_sigma;
		int x0 = (int)init_scale * scale * cos(ele.ori),
			y0 = (int)init_scale * scale * sin(ele.ori);
		int x1 = (int)ele.width + x0, y1 = (int)ele.height + y0;
		img.draw_line(ele.width, ele.height, x1, y1, red);
		// Draw red arrow
		float alpha = 0.33, beta = 0.33;
		float xx0 = ele.width + x0 - alpha * (x0 + beta * y0),
			  yy0 = ele.height + y0 - alpha * (y0 - beta * x0),
			  xx1 = ele.width + x0 - alpha * (x0 - beta * y0),
			  yy1 = ele.height + y0 - alpha * (y0 + beta * x0);
		img.draw_line((int)xx0, (int)yy0, (int)x1, (int)y1, red);
		img.draw_line((int)xx1, (int)yy1, (int)x1, (int)y1, red);
	}
	img.display();
}

// Compute vector's 2-norm
float SIFT::getNormVector(float input[], int dim){
	float result = 0.0;
	for(int i=0; i<dim; i++)
		result += input[i] * input[i];
	return sqrt(result);
}

// 5. Extract key points' feature descriptors
void SIFT::ExtractFeatureDescriptors(){
	float orient_spacing = PI / 4;
	float orient_angles[8] = {-PI, -PI + orient_spacing, -PI/2, -orient_spacing, 0.0,
								orient_spacing, PI/2, PI + orient_spacing};
	float grid[2 * 16];  // Make grids for feat extracting
	// Fill the grid
	for(int i=0; i<4; i++){
		for(int j=0; j<2 * 4; j+=2){
			grid[i * 2 * 4 + j] = -6 + 4 * i;
			grid[i * 2 * 4 + j + 1] = -6 + j * 2;
		}
	}
	// Make samples
	float sample[2 * 256];
	for(int i=0; i<4*4; i++){
		for(int j=0; j<8*4; j+=2){
			sample[i * 8 * 4 + j] = i - (2 * 4 - 0.5f);
			sample[i * 8 * 4 + j + 1] = j/2 - (2 * 4 - 0.5f);
		}
	}
	float windowSize = 2 * 4;
	cout << "Start extracting features, features points length: " << keyDescriptors.size() << endl;
	int count = -1;
	for(KeyPoint ele : this->keyDescriptors){
		count++;
		float tempSin = sin(ele.ori), tempCos = cos(ele.ori);
		float center[2 * 16];
		for(int i=0; i<4; i++){
			for(int j=0; j<2*4; j+=2){
				float x = grid[i * 2 * 4 + j],
					  y = grid[i * 2 * 4 + j + 1];
				center[i * 2 * 4 + j] = (tempCos * x + tempSin * y + ele.x);
				center[i * 2 * 4 + j + 1] = (-tempSin * x + tempCos * y + ele.y);
			}
		}
		float feat[2 * 256];
		for(int i=0; i<64 * 4; i+=2){
			float t0 = sample[i], t1 = sample[i+1];
			feat[i] = (tempCos * t0 + tempSin * t1 + ele.x);
			feat[i+1] = (-tempSin * t0 + tempCos * t1 + ele.y);
		}
		// Init feature desctiptors
		float desc[128] = {0.0};
		for(int i=0; i<512; i+=2){
			float sx = feat[i], sy = feat[i+1];
			// Interpolcation of nearest-k neighbors
			float st1 = getPixelBi(this->octaves[ele.octave_order].octave[ele.level_order].level, sx, sy-1),
				  st2 = getPixelBi(this->octaves[ele.octave_order].octave[ele.level_order].level, sx-1, sy),
				  st3 = getPixelBi(this->octaves[ele.octave_order].octave[ele.level_order].level, sx+1, sy),
				  st4 = getPixelBi(this->octaves[ele.octave_order].octave[ele.level_order].level, sx, sy+1);
			// Compute differs, magniture and orientation
			float diff[8] = {0.0};
			float diff_x = st2 - st3, diff_y = st4 - st1;
			float tempMag = sqrt(diff_x * diff_x + diff_y * diff_y), tempGrad = atan(diff_y / diff_x);
			if(tempGrad == PI)
				tempGrad = -PI;
			// Compute weights
			float xWeights[4 * 4], yWeights[4 * 4], posWeights[8 * 4 * 4];
			for(int j=0; j<32; j+=2){
				xWeights[j/2] = max(0.0f, 1-(fabs(center[j] - sx / 4.0f)));
				yWeights[j/2] = max(0.0f, 1-(fabs(center[j+1] - sy / 4.0f)));
			}
			for(int a=0; a<16; a++){
				for(int b=0; b<8; b++)
					posWeights[a*8+b] = xWeights[a] * yWeights[a];
			}
			// Compute orientation weights
			float orientWeights[128] = {0.0};
			for(int a=0; a<8; a++){
				float angle = tempGrad - (ele.ori) - orient_angles[a] + PI,
					  halfAngle = angle / (2.0 * PI);
				// Half-Make
				angle = angle - (int)(halfAngle) * (2.0 * PI);
				diff[a] = angle - PI;
			}
			// Compute Gaussian weights
			float gaussTemp = exp(-((sx - ele.x) * (sx - ele.x) + (sy - ele.y) * (sy - ele.y)) 
									/ (2 * windowSize * windowSize)) / (2 * PI * windowSize * windowSize);
			for(int a=0; a<128; a++){
				orientWeights[a] = max(0.0f, (float)(1.0 - fabs(diff[a%8]) / orient_spacing * 1.0f));
				// Compute final desciptors weights
				desc[a] = desc[a] + orientWeights[a] * posWeights[a] * gaussTemp * tempMag;
			}
		}
		cout << "Getting features " << count << endl;
		// Norm descriptors
		float norm = getNormVector(desc, 128);
		for(int a=0; a<128; a++){
			desc[a] /= norm;
			// Contrast
			if(desc[a] > 0.2) desc[a] = 0.2;
		}
		norm = getNormVector(desc, 128);
		for(int a=0; a<128; a++)
			desc[a] /= norm;
		ele.descriptors = desc;
	}
}

 // Using down sampling to make half size
CImg<float> SIFT::makeHalfSize(CImg<float> input){
	int halfWidth = input.width() / 2, halfHeight = input.height() / 2;
	CImg<float> halfImg(halfWidth, halfHeight, 1, 3, 1);
	cimg_forXY(halfImg, x, y){
		halfImg(x, y) = input(x * 2, y * 2); 
	}
	return halfImg;
}

// Using up sampling to make double size
CImg<float> SIFT::makeDoubleSize(CImg<float> input){
	int doubleWidth = input.width() * 2, doubleHeight = input.height() * 2;
	CImg<float> doubleImg(doubleWidth, doubleHeight, 1, 3, 1);
	cimg_forXY(doubleImg, x, y){
		doubleImg(x, y) = input(x / 2, y / 2);
	}
	return doubleImg;
}

// Get 2D gaussian kernel
CImg<float> SIFT::make2DGaussianKernel(float sigma){
	int dim = (int)max(3.0f, (float)(2.0 * GAUSSKERN * sigma + 1.0f));
	// Make dim no even
	if(dim % 2 == 0) dim++;
	// Make kernel
	CImg<float> result(dim, dim, 1, 1, 0);
	float m = 1.0f / (sqrt(2.0 * PI) * sigma);
	for(int i=0; i<(dim+1)/2; i++){
		for(int j=0; j<(dim+1)/2; j++){
			float v = m * exp(-(1.0f*i*i + 1.0f*j*j) / (2.0f * sigma * sigma));
			result(j + dim/2, i + dim/2) = v;
			result(-j + dim/2, i + dim/2) = v;
			result(j + dim/2, -i + dim/2) = v;
			result(-j + dim/2, -i + dim/2) = v;
		}
	}
	return result;
}
// Bilinear interpolation
float SIFT::getPixelBi(CImg<float> input, int col, int row){
	int tempX = col, tempY = row;

	if(tempX < 0 || tempX >= input.width() 
		|| tempY < 0 || tempY >= input.height())
		return 0;
	// Border check and reduce
	if(col > input.width() - 1)
		col = input.width() - 1;
	if(row > input.height() - 1)
		row = input.height() - 1;
	float res1 = 0.0, res2 = 0.0;
	float fracX = 1.0 - (col - tempX),
		  fracY = 1.0 - (row - tempY);
	if(fracX < 1){
		res1 = fracX * input(tempX, tempY) + (1.0 - fracX) * input(tempX + 1, tempY);
	}else {
		res1 = input(tempX, tempY);
	}
	if(fracY < 1){
		if(fracX < 1){
			res2 = fracX * input(tempX, tempY+1) + (1.0 - fracX) * input(tempX+1, tempY+1);
		}else {
			res2 = input(tempX, tempY+1);
		}
	}
	return fracY * res1 + (1.0 - fracY) * res2;
}

// Pyrmids associate functions
CImg<float> SIFT::ImgJoinHorizen(CImg<float> img1, CImg<float> img2){
	CImg<float> result(img1.width() + img2.width(), max(img1.height(), img2.height()), 1, 1, 0);
	cimg_forXY(img1, x, y){
		result(x, y) = img1(x, y);
	}
	cimg_forXY(img2, x, y){
		result(x + img1.width(), y) = img2(x, y);
	}
	return result;
}

CImg<float> SIFT::ImgJoinVertical(CImg<float> img1, CImg<float> img2){
	CImg<float> result(max(img1.width(), img2.width()), img1.height() + img2.height(), 1, 1, 0);
	cimg_forXY(img1, x, y){
		result(x, y) = img1(x, y);
	}
	cimg_forXY(img2, x, y){
		result(x, y + img1.height()) = img2(x, y);
	}
	return result;
}

// Covert and normalization
CImg<float> SIFT::convertScale(CImg<float> src, float scale, float shift){
	CImg<float> result(src.width(), src.height(), 1, 1, 0);
	cimg_forXY(src, x, y){
		result(x, y) = src(x, y) * scale + shift;
	}
	return result;
}

CImg<float> SIFT::toGrayImage(CImg<float> img){
	CImg<float> result(img.width(), img.height(), 1, 1, 0);
	cimg_forXY(result, x, y){
		result(x, y) = 0.2126 * img(x, y, 0) + 0.7152 * img(x, y, 1) + 0.0722 * img(x, y, 2);
	}
	return result;
}

// Main process startup functions
void SIFT::SIFT_Start(){
	// To gray
	CImg<float> grayImg = toGrayImage(this->srcImg);
	// Convert
	grayImg = convertScale(grayImg, 1.0 / 255, 0);
	int dim = min(grayImg.width(), grayImg.height());
	this->octavesNum = (int)(log((float)dim) / log(2.0)) - 2;
	this->octavesNum = min(octavesNum, MAXOCTAVES);
	// Step 1. Filtering noises and init pyrmids
	CImg<float> initImg = InitImage(grayImg);
	cout << "fuck" << endl;
	// Step 2. Make DOG pyramids and Gaussian pyramids
	BuildGaussianOctave(initImg);
	// Show Gaussian pyramids
	cout << "fuck" << endl;
	// Show DOG pyramids

	// Step 3. Detect features pointss
	DetectKeyPoints();
	cout << "fuck" << endl;
	this->keyPointsImg = srcImg;
	DisplayKeyPoints();
	this->keyPointsImg.display();
	// Step 4. Compute gradients and main orientations of keyPoints
	ComputeGrad_Mag_Dir();
	AssignTheMainOrientation();
	CImg<float> ttemp = srcImg;
	DisplayOrientation(ttemp);
	// Step 5. Extract key points descriptors
	ExtractFeatureDescriptors();

}

vector<KeyPoint> SIFT::GetFirstKeyDescriptor(){
	return this->keyDescriptors;
}

void SIFT::saveKeyPointsImg(char* fileName){
	this->keyPointsImg.save(fileName);
}