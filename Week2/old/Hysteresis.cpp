/*
*  PROGRAM: Hysteresis.cpp
*  PURPOSE: Provides all readlizations of hysteresis class
*/
#include "Hysteresis.h"

Hysteresis::Hysteresis(CImg<unsigned char>& srcImg_, float tlow_, float thigh_, CImg<float>& angles_){
	srcImg = srcImg_;
	tlow = tlow_;
	thigh = thigh_;
	angles = angles_;
}

/*
*  Non_max_suppression methods, provides nms to magnitude of
*  gradient image.
*/
void Hysteresis::non_max_supp(){
	CImg<unsigned char> temp(srcImg.width(), srcImg.height(), 1, 1);
    for (int i=1; i< srcImg.height() - 1; i++) {
        for (int j=1; j<srcImg.width() - 1; j++) {
            float ttmp = angles(j,i);

            temp(j-1, i-1) = srcImg(j,i);
            //Horizontal Edge
            if (((-22.5 < ttmp) && (ttmp <= 22.5)) || ((157.5 < ttmp) && (ttmp <= -157.5)))
            {
                if ((srcImg(j,i) < srcImg(j,i+1)) || (srcImg(j,i) < srcImg(j,i-1)))
                    temp(j-1, i-1) = 0;
            }
            //Vertical Edge
            if (((-112.5 < ttmp) && (ttmp <= -67.5)) || ((67.5 < ttmp) && (ttmp <= 112.5)))
            {
                if ((srcImg(j,i) < srcImg(j+1,i)) || (srcImg(j,i) < srcImg(j-1,i)))
                    temp(j-1, i-1) = 0;
            }
            
            //-45 Degree Edge
            if (((-67.5 < ttmp) && (ttmp <= -22.5)) || ((112.5 < ttmp) && (ttmp <= 157.5)))
            {
                if ((srcImg(j,i) < srcImg(j-1,i+1)) || (srcImg(j,i) < srcImg(j+1,i-1)))
                    temp(j-1, i-1) = 0;
            }
            
            //45 Degree Edge
            if (((-157.5 < ttmp) && (ttmp <= -112.5)) || ((22.5 < ttmp) && (ttmp <= 67.5)))
            {
                if ((srcImg(j,i) < srcImg(j+1,i+1)) || (srcImg(j,i) < srcImg(j-1,i-1)))
                    temp(j-1, i-1) = 0;
            }
        }
    }
    srcImg = temp;
}
/*
*  follow_edges methods provides recursive routine to trace edges along all path
*/
void Hysteresis::follow_edges(unsigned char* edgemapptr, short* edgemagptr, short lowval, int cols){
	short* temp_mag_ptr;
	unsigned char* temp_map_ptr;
	float thresh;
	int x[8] = {1,1,0,-1,-1,-1,0,1},
		y[8] = {0,1,1,1,0,-1,-1,-1};

	for(int i=0; i<8; i++){
		temp_map_ptr = edgemapptr - y[i]*cols + x[i];
		temp_mag_ptr = edgemagptr - y[i]*cols + x[i];
		if((*temp_map_ptr == POSSIBLE_EDGE) && (*temp_mag_ptr > lowval)){
			*temp_map_ptr = (unsigned char) EDGE;
			follow_edges(temp_map_ptr, temp_mag_ptr, lowval, cols);
		}
	}
}

/*
*  Apply hysteresis to get result edge image
*/
CImg<unsigned char> Hysteresis::apply_hysteresis(){
	int low = tlow * 100, high = thigh * 100;
	CImg<unsigned char> edge(srcImg.width(), srcImg.height(), 1, 1);

	cimg_forXY(srcImg, x, y){
		edge(x, y) = srcImg(x, y);
		if(edge(x, y) > high)
			edge(x, y) = 100;
		else if(edge(x, y) < low)
			edge(x, y) = 0;
		else {
			bool higher = false, inside = false;
			for(int i=y-1; i<y-2; i++){
				for(int j=x-1; j<x+2; j++){
					if(i <= 0 || j <= 0 || i > edge.width() || j > edge.height()) continue;
					else {
						if(edge(i, j) > high){
							edge(i, j) = 100;
							higher = true;
							break;
						}else if(edge(i, j) <= high && edge(i, j) >= low){
							inside = true;
						}
					}
				}
				if(higher) break;
			}
			if(!higher && inside){
				for(int i=y-2; i<y+3; i++){
					for(int j=x-1; j<x+3; j++){
						if(i <= 0 || j <= 0 || i > edge.width() || j > edge.height()) continue;
						else {
							if(edge(i, j) > high){
								edge(i, j) = 100;
								higher = true;
								break;
							}
						}
					}
					if(higher) break;
				}
				if(!higher) edge(x, y) = 0;
			}
		}
	}

	return edge;
}
