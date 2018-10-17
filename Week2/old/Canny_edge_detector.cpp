/*
*  PROGRAM: Canny_edge_detector.cpp
*  PURPOSE: Finish and build all the methods used in edge detector.
*/

#include "Canny_edge_detector.h"
#include "Hysteresis.h"

Edge_detector::Edge_detector(CImg<unsigned char>& image, float sigma, float tlow, float thigh){
	srcImg = image;
	edgeImg = image;
	this->sigma = sigma;
	this->tlow = tlow;
	this->thigh = thigh;
}

CImg<unsigned char>& Edge_detector::edge_detection(){
	int rows = srcImg.height(), cols = srcImg.width();

	  /*****************************************
   	* Perform gaussian smoothing on the 
   	* image using the input standard deviation.
   	******************************************/
	  gaussian_smooth();

	  /******************************************
   	* Use sobel methods to find gradients
   	*******************************************/
   	sobel();

    /******************************************
   	* Create a new hysteresis object
   	*******************************************/
   	Hysteresis hyster(edgeImg, tlow, thigh, angles);

   	/******************************************
   	* Perform non-maximal suppression
   	*******************************************/
    hyster.non_max_supp();

    /******************************************
   	* Use hysteresis to mark the edge pixels
   	*******************************************/
   	edgeImg = hyster.apply_hysteresis();
    //remove_less20_edges();
   	edgeImg.save("lenag.bmp");
   	return edgeImg;
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
            if(x < 0 || y < 0 || x >= edgeImg.width() || y >= edgeImg.height())
              continue;
            else if(!visited(x, y) && edgeImg(x, y) == 255){
              store.push(make_pair(x, y));
              visited(x, y) = true;
            }
          }
        }
      }
      if(path.size() < 50){
        for(int i=0; i<path.size(); i++)
          edgeImg(path[i].first, path[i].second) = 0;
      }
    }
  }
}

/*
*  PROCEDURE: gaussian_smooth
*  Blur the source image with gaussian filter
*/
void Edge_detector::gaussian_smooth(){
	vector<vector<double>> filter = make_gaussian_filter(3, 3);
	int size = (int)filter.size() / 2;
	for(int i=size; i<srcImg.height() - size; i++){
		for(int j=size; j<srcImg.width() - size; j++){
			double sum = 0;
			for(int x=0; x<filter.size(); x++){
				for(int y=0; y<filter.size(); y++)
					sum += filter[x][y] * (double)srcImg(j + y - size, i + x - size);
			}
			edgeImg(j-size, i-size) = sum;
		}
	}
}

vector<vector<double>> Edge_detector::make_gaussian_filter(int rows, int cols){
	vector<vector<double>> filter;

	for (int i = 0; i < rows; i++)
	{
        vector<double> col;
        for (int j = 0; j < cols; j++)
        {
            col.push_back(-1);
        }
		filter.push_back(col);
	}

	float coordSum = 0;
	float constant = 2.0 * sigma * sigma;

	// Sum is for normalization
	float sum = 0.0;

	for (int x = - rows/2; x <= rows/2; x++)
	{
		for (int y = -cols/2; y <= cols/2; y++)
		{
			coordSum = (x*x + y*y);
			filter[x + rows/2][y + cols/2] = (exp(-(coordSum) / constant)) / (3.1415926 * constant);
			sum += filter[x + rows/2][y + cols/2];
		}
	}

	// Normalize the Filter
	for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            filter[i][j] /= sum;

	return filter;

}

void Edge_detector::sobel(){
	//Sobel X Filter
    double x1[] = {-1.0, 0, 1.0};
    double x2[] = {-2.0, 0, 2.0};
    double x3[] = {-1.0, 0, 1.0};

    vector<vector<double>> xFilter(3);
    xFilter[0].assign(x1, x1+3);
    xFilter[1].assign(x2, x2+3);
    xFilter[2].assign(x3, x3+3);
    
    //Sobel Y Filter
    double y1[] = {1.0, 2.0, 1.0};
    double y2[] = {0, 0, 0};
    double y3[] = {-1.0, -2.0, -1.0};
    
    vector<vector<double>> yFilter(3);
    yFilter[0].assign(y1, y1+3);
    yFilter[1].assign(y2, y2+3);
    yFilter[2].assign(y3, y3+3);
    
    //Limit Size
    int size = (int)xFilter.size()/2;

    CImg<float> aa(edgeImg.width(), edgeImg.height(), 1, 1);
    angles = aa;

    for(int i=size; i<edgeImg.height()-size; i++){
    	for(int j=size; j<edgeImg.width()-size; j++){
    		double sumx = 0;
            double sumy = 0;
            
			for (int x = 0; x < xFilter.size(); x++)
				for (int y = 0; y < xFilter.size(); y++){
                    sumx += xFilter[x][y] * (double)edgeImg(j + y - size, i + x - size); //Sobel_X Filter Value
                    sumy += yFilter[x][y] * (double)edgeImg(j + y - size, i + x - size); //Sobel_Y Filter Value
				}

            double sumxsq = sumx*sumx;
            double sumysq = sumy*sumy;
            
            double sq2 = sqrt(sumxsq + sumysq);
            
            if(sq2 > 255) //Unsigned Char Fix
                sq2 =255;
            edgeImg(j-size, i-size) = sq2;
 			
 			if(sumx == 0)
 				angles(j-size, i-size) = 90;
 			else
 				angles(j-size, i-size) = atan(sumy/sumx)*(180/3.14);
    	}
    }
}


void Edge_detector::usage(char* progName){
	fprintf(stderr,"\n<USAGE> %s image sigma tlow thigh [writedirim]\n",progName);
    fprintf(stderr,"\n      image:      An image to process. Must be in ");
    fprintf(stderr,"PGM format.\n");
  	fprintf(stderr,"      sigma:      Standard deviation of the gaussian");
  	fprintf(stderr," blur kernel.\n");
  	fprintf(stderr,"      tlow:       Fraction (0.0-1.0) of the high ");
  	fprintf(stderr,"edge strength threshold.\n");
  	fprintf(stderr,"      thigh:      Fraction (0.0-1.0) of the distribution");
  	fprintf(stderr," of non-zero edge\n                  strengths for ");
  	fprintf(stderr,"hysteresis. The fraction is used to compute\n");
  	fprintf(stderr,"                  the high edge strength threshold.\n");
  	fprintf(stderr,"      writedirim: Optional argument to output ");
  	fprintf(stderr,"a floating point");
  	fprintf(stderr," direction image.\n\n");
}