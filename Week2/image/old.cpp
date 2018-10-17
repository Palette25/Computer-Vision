float *kernel;  // A one dimensional gaussian kernel
	int windowsize, center, rows = srcImg.height(), cols = srcImg.width();
	float dot, sum;
	float* tempBuf;  // Buffer for storing result of x-dir bluring
	/******************************************
   	* Create 1-dim gaussian smoothing kernel
   	*******************************************/
	make_gaussian_kernel(&kernel, windowsize);
	center = windowsize / 2;

	if((tempBuf = (float*) calloc(rows*cols, sizeof(float))) == NULL){
      fprintf(stderr, "Error allocating the buffer image.\n");
      exit(3);
   }

	/******************************************
   	* Blur in the x - direction
   	*******************************************/
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			dot = sum = 0.0;
			for(int k=-center; k<=center; k++){
				if((k+j) >=0 && (k+j) <cols){
					dot += (float)srcImg(j*cols+i+k) * kernel[center+k];
					sum += kernel[center+k];
				}
			}
			tempBuf[i*cols+j] = dot/sum;
		}
	}

	/******************************************
   	* Blur in the y - direction
   	*******************************************/
	for(int j=0; j<cols; j++){
		for(int i=0; i<rows; i++){
			dot = sum = 0.0;
			for(int k=-center; k<=center; k++){
				if(k+i >=0 && k+i < rows){
					dot += tempBuf[(k+i)*cols+j] * kernel[center+k];
					sum += kernel[center+k];
				}
			}
			edgeImg(j*cols+i) = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);
		}
	}

	free(tempBuf);
	free(kernel);



void Edge_detector::derrivative_x_y(short int** delta_x, short int** delta_y){
	int pos, rows = srcImg.height(), cols = srcImg.width();
	/******************************************
   	* Allocate spaces to store x, y derivatives
   	*******************************************/
   	if(((*delta_x) = (short*) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the delta_x image.\n");
      exit(5);
   }
   if(((*delta_y) = (short*) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the delta_x image.\n");
      exit(5);
   }

   /******************************************
   * Computing x-derivative
   *******************************************/
   for(int x=0; x<rows; x++){
   	  pos = x * cols;
   	  (*delta_x)[pos] = edgeImg(pos+1) - edgeImg(pos);
   	  ++pos;
   	  for(int y=1; y<cols-1; y++, pos++){
   	  	  (*delta_x)[pos] = edgeImg(pos+1) - edgeImg(pos-1);
   	  }
   	  (*delta_x)[pos] = edgeImg(pos) - edgeImg(pos-1);
   }

   /******************************************
   * Computing y-derivative
   *******************************************/
   for(int y=0; y<cols; y++){
   	  pos = y;
   	  (*delta_y)[pos] = edgeImg(pos+cols) - edgeImg(pos);
   	  pos += cols;
   	  for(int x=1; x<rows-1; x++, pos+=cols){
   	  	  (*delta_y)[pos] = edgeImg(pos+cols) - edgeImg(pos-cols);
   	  }
   	  (*delta_y)[pos] = edgeImg(pos) - edgeImg(pos-cols);
   }
}

void Edge_detector::magnitude_x_y(short int* delta_x, short int* delta_y, short int** magnitude){
	int pos = 0, rows = srcImg.height(), cols = srcImg.width();
	int sq1, sq2;
	/******************************************
   	* Allocate spaces to store magnitude
   	*******************************************/
   	if((*magnitude = (short*) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the magnitude image.\n");
      exit(6);
   	}

   	for(int x=0; x<rows; x++){
   		for(int y=0; y<cols; y++, pos++){
   			sq1 = (int)delta_x[pos] * (int)delta_x[pos];
         	sq2 = (int)delta_y[pos] * (int)delta_y[pos];
         	(*magnitude)[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));
   		}
   	}
   	
}


int center;
	float x, fx, sum = 0.0;
	// Define windowSize and center of the kernel
	windowsize = 1 + 2 * ceil(2.5 * this->sigma);
	center = windowsize / 2;
	// Allocate kernel space
	if((*kernel = (float*) calloc(windowsize, sizeof(float))) == NULL){
      fprintf(stderr, "Error callocing the gaussian kernel array.\n");
      exit(4);
   	}

   	// Fill kernel
   	for(int i=0; i<windowsize; i++){
   		x = (float)(i - center);
   		fx = pow(2.71828, -0.5*x*x/(this->sigma*this->sigma)) / (this->sigma * sqrt(6.2831853)); // Calculating fx
   		(*kernel)[i] = fx;
   		sum += fx;
   	}

   	// Divide kernel with sum of all fx
   	for(int i=0; i<windowsize; i++)
   		(*kernel)[i] /= sum;

/*
*  Non_max_suppression methods, provides nms to magnitude of
*  gradient image.
*/
void Hysteresis::non_max_supp(short int* gradx, short int* grady){
   short *mag_row_ptr, *mag_ptr;
   short *gx_row_ptr, *gx_ptr;
   short *gy_row_ptr, *gy_ptr;
   short z1, z2, gx, gy, mz;
   float mag1, mag2, x_perp, y_perp;
   unsigned char *result_row_ptr, *result_ptr;

   /******************************************
      * Zero the edges of the result image
      *******************************************/
      result_row_ptr = nms;
   result_ptr = nms + cols * (rows - 1);
   // In x-dir
   for(int i=0; i<cols; i++, result_row_ptr++, result_ptr++){
      *result_row_ptr = *result_ptr = (unsigned char) 0;
   }

   result_ptr = nms;
   result_row_ptr = nms + cols - 1;
   // In y-dir
   for(int i=0; i<rows; i++, result_row_ptr+=cols, result_ptr+=cols){
      *result_row_ptr = *result_ptr = (unsigned char) 0;
   }

   /******************************************
      * Suppress non-maximun points
      *******************************************/
   mag_row_ptr = mag + cols + 1;
   gx_row_ptr = gradx + cols + 1;
   gy_row_ptr = grady + cols + 1;
   result_row_ptr = nms + cols + 1;
   int j = 1;
   for(int i=1; i<rows-2; i++, mag_row_ptr+=cols, gx_row_ptr+=cols, 
                     gy_row_ptr+=cols, result_row_ptr+=cols){
      for(mag_ptr=mag_row_ptr, gx_ptr=gx_row_ptr, gy_ptr=gy_row_ptr,
                     result_ptr=result_row_ptr; j<cols-2;
                        j++, mag_ptr++, gx_ptr++, gy_ptr++, result_ptr++){
         mz = *mag_ptr;
         if(mz == 0){
            *result_ptr = (unsigned char) NOEDGE;
         }else {
            x_perp = -(gx = *gx_ptr)/((float)mz);
            y_perp = (gy = *gy_ptr)/((float)mz);
         }

         if(gx >= 0){
            if(gy >= 0){
               if(gx >= gy){
                  // 111
                  // The left point
                  z1 = *(mag_ptr - 1);
                  z2 = *(mag_ptr - cols - 1);
                  mag1 = (mz - z1)*x_perp + (z2 - z1)*y_perp;
                  // The right point
                  z1 = *(mag_ptr + 1);
                  z2 = *(mag_ptr + cols + 1);
                  mag2 = (mz - z1)*x_perp + (z2 - z1)*y_perp;
               }else {
                  // 110
                  // The left point
                  z1 = *(mag_ptr - cols);
                  z2 = *(mag_ptr - cols - 1);
                  mag1 = (z1 - z2)*x_perp + (z1 - mz)*y_perp;
                  // The right point
                  z1 = *(mag_ptr + cols);
                  z2 = *(mag_ptr + cols + 1);
                  mag2 = (z1 - z2)*x_perp + (z1 - mz)*y_perp;
               }
            }else {
               if(gx >= -gy){
                  // 101
                  // The left point
                  z1 = *(mag_ptr - 1);
                  z2 = *(mag_ptr + cols - 1);
                  mag1 = (mz - z1)*x_perp + (z1 - z2)*y_perp;
                  // The right point
                  z1 = *(mag_ptr + 1);
                  z2 = *(mag_ptr - cols + 1);
                  mag2 = (mz - z1)*x_perp + (z1 - z2)*y_perp;
               }else {
                  // 100
                  // The left point
                  z1 = *(mag_ptr + cols);
                  z2 = *(mag_ptr + cols - 1);
                  mag1 = (z1 - z2)*x_perp + (mz - z1)*y_perp;
                  // The right point
                  z1 = *(mag_ptr - cols);
                  z2 = *(mag_ptr - cols + 1);
                  mag2 = (z1 - z2)*x_perp + (mz - z1)*y_perp;
               }
            }
         }else {
            if((gy = *gy_ptr) >= 0){
               if(-gx >= gy){
                  // 011
                  // The left point
                  z1 = *(mag_ptr + 1);
                  z2 = *(mag_ptr -cols + 1);
                  mag1 = (z1 - mz)*x_perp + (z2 - z1)*y_perp;
                  // The right point
                  z1 = *(mag_ptr - 1);
                  z2 = *(mag_ptr + cols - 1);
                  mag2 = (z1 - mz)*x_perp + (z2 - z1)*y_perp;
               }else {
                  // 010
                  // The left point
                  z1 = *(mag_ptr - cols);
                  z2 = *(mag_ptr - cols + 1);
                  mag1 = (z2 - z1)*x_perp + (z1 - mz)*y_perp;
                  // The right point
                  z1 = *(mag_ptr + cols);
                  z2 = *(mag_ptr + cols - 1);
                  mag2 = (z2 - z1)*x_perp + (z1 - mz)*y_perp;
               }
            }else {
               if(-gx > -gy){
                  // 001
                  // The left point
                  z1 = *(mag_ptr + 1);
                  z2 = *(mag_ptr + cols + 1);
                  mag1 = (z1 - mz)*x_perp + (z1 - z2)*y_perp;
                  // The right point
                  z1 = *(mag_ptr - 1);
                  z2 = *(mag_ptr - cols - 1);
                  mag2 = (z1 - mz)*x_perp + (z1 - z2)*y_perp;
               }else {
                  // 000
                  // The left point
                  z1 = *(mag_ptr + cols);
                  z2 = *(mag_ptr + cols + 1);
                  mag1 = (z2 - z1)*x_perp + (mz - z1)*y_perp;
                  // The right point
                  z1 = *(mag_ptr - cols);
                  z2 = *(mag_ptr - cols - 1);
                  mag2 = (z2 - z1)*x_perp + (mz - z1)*y_perp;
            }
         }
      }
      // Determine whether the point is maximum point
      if(mag1 > 0.0 || mag2 > 0.0){
         *result_ptr = (unsigned char) NOEDGE;
      }else {
         if(mag2 == 0.0)
            *result_ptr = (unsigned char) NOEDGE;
         else
            *result_ptr = (unsigned char) POSSIBLE_EDGE;
      }
   }
}
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
CImg<unsigned char> Hysteresis::apply_hysteresis(CImg<unsigned char> src){
   int pos = 0, highcount, lowcount;
   CImg<unsigned char> res = src;
   int hist[32768];
   // Loop to copy values to res
   for(int i=0, pos=0; i<rows; i++){
      for(int j=0; j<cols;j++, pos++){
         if(nms[pos] == POSSIBLE_EDGE) res(pos) = POSSIBLE_EDGE;
         else res(pos) = NOEDGE;
      }
   }
   for(int i=0, pos=0; i<rows; i++, pos+=cols){
      res(pos) = NOEDGE;
      res(pos+cols-1) = NOEDGE;
   }
   pos = (rows-1) * cols;
   for(int j=0; j<cols; j++, pos++){
      res(j) = NOEDGE;
      res(pos) = NOEDGE;
   }

   // Compute the histogram of the magnitude image
   for(int i=0; i<32768; i++){
      hist[i] = 0;
   }
   for(int i=0, pos=0; i<rows; i++){
      for(int j=0; j<cols; j++, pos++){
         if(res(pos) == POSSIBLE_EDGE) hist[mag[pos]]++;
      }
   }

   // Compute the number of pixels that passed the n-m-s
   int max_mag = 0, nm = 0;
   for(int i=1; i<32768; i++){
      if(hist[i] != 0) max_mag = i;
      nm += hist[i];
   }
   highcount = (int)(nm * thigh + 0.5);

   /****************************************************************************
   * Compute the high threshold value as the (100 * thigh) percentage point
   * in the magnitude of the gradient histogram of all the pixels that passes
   * non-maximal suppression. Then calculate the low threshold as a fraction
   * of the computed high threshold value. John Canny said in his paper
   * "A Computational Approach to Edge Detection" that "The ratio of the
   * high to low threshold in the implementation is in the range two or three
   * to one." That means that in terms of this implementation, we should
   * choose tlow ~= 0.5 or 0.33333.
   ****************************************************************************/
   int i = 1, highthres = 0, lowthres = 0;
   nm = hist[1];
   while((i<max_mag-1) && (nm < highcount)){
      i++;
      nm += hist[i];
   }
   highthres = i;
   lowthres = (int)(highthres * tlow + 0.5);

   /* Use loop to look for pixels above highthreshold 
   *  and use follow_edges to continue the edge
   */
   for(int i=0, pos=0; i<rows; i++){
      for(int j=0; j<cols; j++, pos++){
         if((res(pos) == POSSIBLE_EDGE) && (mag[pos] >= highthres)){
            res(pos) = EDGE;
            follow_edges(res, (mag+pos), lowthres, cols);
         }
      }
   }

   for(int i=0, pos=0; i<rows; i++){
      for(int j=0; j<cols; j++, pos++){
         if(res(pos) != EDGE)
            res(pos) = NOEDGE;
      }
   }

   return res;
}
