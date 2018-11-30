#include "Utils.h"
#include <assert.h>

unsigned char Utils::bilinearInterpolate(CImg<unsigned char>& image, float x, float y, int chanNum){
	int x_pos = floor(x);
    float x_u = x - x_pos;
    int xb = (x_pos < image.width() - 1) ? x_pos + 1 : x_pos;

    int y_pos = floor(y);
	float y_v = y - y_pos;
    int yb = (y_pos < image.height() - 1) ? y_pos + 1 : y_pos;

	float P1 = image(x_pos, y_pos, chanNum) * (1 - x_u) + image(xb, y_pos, chanNum) * x_u;
	float P2 = image(x_pos, yb, chanNum) * (1 - x_u) + image(xb, yb, chanNum) * x_u;

    return P1 * (1 - y_v) + P2 * y_v;
}

CImg<unsigned char> Utils::cylinderizeImg(CImg<unsigned char>& src){
	int projection_width, projection_height;
	CImg<unsigned char> res(src.width(), src.height(), 1, src.spectrum(), 0);
	float r;

	if (src.width() > src.height()) {
		projection_width = src.height();
		projection_height = src.width();

		r = (projection_width / 2.0) / tan(15.0 * PI / 180.0);

		for (int i = 0; i < res.width(); i++) {
			for (int j = 0; j < res.height(); j++) {
				float dst_x = j - projection_width / 2;
				float dst_y = i - projection_height / 2;

				float k = r / sqrt(r * r + dst_x * dst_x);
				float src_x = dst_x / k;
				float src_y = dst_y / k;

				if (src_x + projection_width / 2 >= 0 && src_x + projection_width / 2 < src.height()
					&& src_y + projection_height / 2 >= 0 && src_y + projection_height / 2 < src.width()) {
					for (int k = 0; k < res.spectrum(); k++) {
						res(i, j, k) = bilinearInterpolate(src, src_y + projection_height / 2, src_x + projection_width / 2, k);
					}
				}
			}
		}
	}
	else {
		projection_width = src.width();
		projection_height = src.height();

		r = (projection_width / 2.0) / tan(15 * PI / 180.0);

		for (int i = 0; i < res.width(); i++) {
			for (int j = 0; j < res.height(); j++) {
				float dst_x = i - projection_width / 2;
				float dst_y = j - projection_height / 2;

				float k = r / sqrt(r * r + dst_x * dst_x);
				float src_x = dst_x / k;
				float src_y = dst_y / k;

				if (src_x + projection_width / 2 >= 0 && src_x + projection_width / 2 < src.width()
					&& src_y + projection_height / 2 >= 0 && src_y + projection_height / 2 < src.height()) {
					for (int k = 0; k < res.spectrum(); k++) {
						res(i, j, k) = bilinearInterpolate(src, src_x + projection_width / 2, src_y + projection_height / 2, k);
					}
				}
			}
		}
	}
	return res;
}

CImg<unsigned char> Utils::toGrayImage(CImg<unsigned char> src){
	// Judge whether is gray or not
	if(src.spectrum() == 1){
		return src;
	}
	CImg<unsigned char> result(src.width(), src.height(), src.depth(), 1);
	cimg_forXY(result, x, y){
		result(x, y, 0, 0) = 0.299 * src(x, y, 0, 0) + 0.587 * src(x, y, 0, 1) + 0.114 * src(x, y, 0, 2);
	}
	return result;
}