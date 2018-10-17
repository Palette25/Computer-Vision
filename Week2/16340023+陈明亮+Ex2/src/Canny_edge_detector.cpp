#include "Canny_edge_detector.h"

Edge_detector::Edge_detector(CImg<unsigned char>& image, float sigma_, float tlow_, float thigh_){
	srcImg = image;
	height = image.height();
	width = image.width();
	sigma = sigma_;
	tlow = tlow_ * MAG_SCALE + 0.5f;
	thigh = thigh_ * MAG_SCALE + 0.5f;
}

Edge_detector::~Edge_detector(){
	if(edgeImg) delete edgeImg;
	if(mag) delete mag;
}

CImg<unsigned char>& Edge_detector::edge_detection(){
	// Allocate buffers
	edgeImg = new int[width * height];
	mag = new int[width * height];
	/*****************************************
   	* Perform gaussian smoothing on the 
   	* image using the input standard deviation.
   	******************************************/
	gaussian_smooth();

	/******************************************
   	* Use sobel methods to find gradients
   	*******************************************/
   	compute_gradients();

    /******************************************
   	* Apply non-maximal-suppresion and hysteresis
   	*******************************************/
   	apply_hysteresis();

   	edge = CImg<unsigned char>(width, height, 1, 1, 0);
   	cimg_forXY(edge, x, y){
   		int tmp = y*width + x;
   		edge(x, y) = edgeImg[tmp] > 0 ? 255 : 0;
   	}
   	

   	remove_less20_edges();
   	edge.display();

   	edge.save("lenag.bmp");

   	return edge;

}

void Edge_detector::remove_less20_edges(){
	queue<pair<int, int> > store;
	CImg<bool> visited(srcImg.width(), srcImg.height(), 1, 1, 0);
	for(int i=0; i<srcImg.width(); i++){
		for(int j=0; j<srcImg.height(); j++){
			if(visited(i, j) == true) continue;
			store.push(make_pair(i, j));
			vector<pair<int, int> > path;
			while(!store.empty()){
				int xx = store.front().first, yy = store.front().second;
				path.push_back(make_pair(xx, yy));
				store.pop();
				for(int x=xx-1; x<xx+2; x++){
					for(int y=yy-1; y<yy+2; y++){
						if(x < 0 || y < 0 || x >= edge.width() || y >= edge.height())
							continue;
						else if(!visited(x, y) && edge(x, y) > 0){
							store.push(make_pair(x, y));
							visited(x, y) = true;
						}
					}
				}
			}
			if(path.size() < 20){
				for(int i=0; i<path.size(); i++)
					edge(path[i].first, path[i].second) = 0;
			}
		}
	}
}

void Edge_detector::gaussian_smooth(){
	int hist[256] = {0}, re[256];
	int sum = 0, k = 0;
	// Calculate hist array
	cimg_forXY(srcImg, x, y){
		hist[srcImg(x, y)]++;
	}
	for(int i=0; i<256; i++){
		sum += hist[i];
		int temp = (sum * 255) / (width * height);
		for(int j=k+1; j<=temp; j++)
			re[j] = i;
		k = temp;
	}
	// Apply result
	cimg_forXY(srcImg, x, y){
		srcImg(x, y) = re[srcImg(x, y)];
	}
}

// Compute gradient after processing
void Edge_detector::compute_gradients(){
	float k_radius = sigma;
	int k_width = 16;
	int i = 0;

	float* xConv = new float[width * height];
    float* yConv = new float[width * height];
    float* xGradient = new float[width * height];
    float* yGradient = new float[width * height];
    float* kernel = new float[k_width];
    float* diffk = new float[k_width];

    for(i=0; i<k_width; i++){
    	float t1, t2, t3;
    	t1 = gauss((float)i, k_radius);
    	if(t1 <= GAP_NUM && i >= 2) break;
    	t2 = gauss(i - 0.5f, k_radius);
    	t3 = gauss(i + 0.5f, k_radius);
    	kernel[i] = (t1+t2+t3) / 3.0f / (2.0f * 3.14f * k_radius * k_radius);
    	diffk[i] = t3 - t2;
    }

    int iX = i-1, mX = width - i - 1;
    int iY = width * (i-1), mY = width * (height-i-1);

    // Perform convolutions in x and y direction
    for(int x=iX; x<mX; x++){
    	for(int y=iY; y<mY; y+=width){
    		int tmp = x+y;
    		float sumX = srcImg[tmp] * kernel[0];
    		float sumY = sumX;
    		int xOffset = 1;
            int yOffset = width;
            while (xOffset < i)
            {
                sumY += kernel[xOffset] * (srcImg[tmp - yOffset] + srcImg[tmp + yOffset]);
                sumX += kernel[xOffset] * (srcImg[tmp - xOffset] + srcImg[tmp + xOffset]);
                yOffset += width;
                xOffset++;
            }

            yConv[tmp] = sumY;
            xConv[tmp] = sumX;
    	}
    }

    for (int x = iX; x < mX; x++){
        for (int y = iY; y < mY; y += width){
            float sum = 0.0f;
            int index = x + y;
            for (int j = 1; j < i; j++)
                sum += diffk[j] * (yConv[index - j] - yConv[index + j]);

            xGradient[index] = sum;
        }
    }

    for (int x = i; x < width - i; x++){
        for (int y = iY; y < mY; y += width){
            float sum = 0.0f;
            int index = x + y;
            int yOffset = width;
            for (int j = 1; j < i; j++){
                sum += diffk[j] * (xConv[index - yOffset] - xConv[index + yOffset]);
                yOffset += width;
            }

            yGradient[index] = sum;
        }
    }

    iX = i;
    mX = width - i;
    iY = width * i;
    mY = width * (height - i);

    no_max_supp(iX, mX, iY, mY, xGradient, yGradient);
    
    free(kernel);
    free(diffk);
    free(xConv);
    free(yConv);
}

// Apply nms
void Edge_detector::no_max_supp(int iX, int mX, int iY, int mY, float* xGradient, float* yGradient){
	for (int x = iX; x < mX; x++){
        for (int y = iY; y < mY; y += width){
            int index = x + y;
            int indexN = index - width;
            int indexS = index + width;
            int indexW = index - 1;
            int indexE = index + 1;
            int indexNW = indexN - 1;
            int indexNE = indexN + 1;
            int indexSW = indexS - 1;
            int indexSE = indexS + 1;

            float xGrad = xGradient[index];
            float yGrad = yGradient[index];
            float gradMag = hypo(xGrad, yGrad);

            /* perform non-maximal supression */
            float nMag = hypo(xGradient[indexN], yGradient[indexN]);
            float sMag = hypo(xGradient[indexS], yGradient[indexS]);
            float wMag = hypo(xGradient[indexW], yGradient[indexW]);
            float eMag = hypo(xGradient[indexE], yGradient[indexE]);
            float neMag = hypo(xGradient[indexNE], yGradient[indexNE]);
            float seMag = hypo(xGradient[indexSE], yGradient[indexSE]);
            float swMag = hypo(xGradient[indexSW], yGradient[indexSW]);
            float nwMag = hypo(xGradient[indexNW], yGradient[indexNW]);
            float tmp;
            
            int flag = ((xGrad * yGrad <= 0.0f) 
                ? abs(xGrad) >= abs(yGrad)
                ? (tmp = abs(xGrad * gradMag)) >= abs(yGrad * neMag - (xGrad + yGrad) * eMag) /*(3)*/
                && tmp > fabs(yGrad * swMag - (xGrad + yGrad) * wMag) /*(4)*/
                : (tmp = abs(yGrad * gradMag)) >= abs(xGrad * neMag - (yGrad + xGrad) * nMag) /*(3)*/
                && tmp > abs(xGrad * swMag - (yGrad + xGrad) * sMag) /*(4)*/
                : abs(xGrad) >= abs(yGrad) /*(2)*/
                ? (tmp = abs(xGrad * gradMag)) >= abs(yGrad * seMag + (xGrad - yGrad) * eMag) /*(3)*/
                && tmp > abs(yGrad * nwMag + (xGrad - yGrad) * wMag) /*(4)*/
                : (tmp = abs(yGrad * gradMag)) >= abs(xGrad * seMag + (yGrad - xGrad) * sMag) /*(3)*/
                && tmp > abs(xGrad * nwMag + (yGrad - xGrad) * nMag) /*(4)*/
                );
            if (flag){
                mag[index] = (gradMag >= MAG_LIMIT) ? (int)MAG_MAX : (int)(MAG_SCALE * gradMag);
            }
            else{
                mag[index] = 0;
            }
        }
    }
    free(xGradient);
    free(yGradient);
}


void Edge_detector::apply_hysteresis(){
	int tmp = 0;
	memset(edgeImg, 0, width*height*sizeof(int));

	for(int x=0; x<height; x++){
		for(int y=0; y<width; y++){
			if(edgeImg[tmp] == 0 && mag[tmp] >= thigh)
				follow_edges(y, x, tmp);
			tmp++;
		}
	}
}

void Edge_detector::follow_edges(int x, int y, int i){
    int x0 = x == 0 ? x : x - 1;
    int x1 = x == width - 1 ? x : x + 1;
    int y0 = y == 0 ? y : y - 1;
    int y1 = y == height - 1 ? y : y + 1;
    bool follow_flag = false, keep_flag = true;

    edgeImg[i] = mag[i];
    for (int j = x0; j <= x1; j++){
        for (int k = y0; k <= y1; k++){
            int tt = j + k * width;
            if ((k != y || j != x) && edgeImg[tt] == 0 && mag[tt] >= tlow){
            	follow_edges(j, k, tt);
            }
        }
    }
    
}

void Edge_detector::usage(char* progName){
	fprintf(stderr,"\n<USAGE> %s image sigma tlow thigh [writedirim]\n",progName);
    fprintf(stderr,"\n      image:      An image to process. Support all ");
    fprintf(stderr," formats.\n");
  	fprintf(stderr,"      sigma:      Standard deviation of the gaussian");
  	fprintf(stderr," blur kernel.\n");
  	fprintf(stderr,"      tlow:       Fraction of the high ");
  	fprintf(stderr,"edge strength threshold.\n");
  	fprintf(stderr,"      thigh:      Fraction of the distribution");
  	fprintf(stderr," of non-zero edge\n                  strengths for ");
  	fprintf(stderr,"hysteresis. The fraction is used to compute\n");
  	fprintf(stderr,"                  the high edge strength threshold.\n");
}
