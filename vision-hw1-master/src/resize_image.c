#include <math.h>
#include <stdlib.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
	int a = round(x);
	int b = round(y);
	float value = get_pixel(im, a, b, c);
	return value;
}

image nn_resize(image im, int w, int h)
{
	// TODO Fill in (also fix that first line)

	image resized = make_image(w, h, im.c);
	float width_slope = (float) im.w / w;
	float width_const = 0.5*width_slope - 0.5;
	float height_slope = (float) im.h / h;
	float height_const = 0.5*height_slope - 0.5;
	float orig_width, orig_height;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			for (int k = 0; k < im.c; k++) {
				orig_width = width_slope*i + width_const;
				orig_height = height_slope*j + height_const;
				float value = nn_interpolate(im, orig_width, orig_height, k);
				set_pixel(resized, i, j, k, value);
			}
		}
	}
    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
	int a = (int)floor(x);
	int b = (int)floor(y);
	float val1 = get_pixel(im, a, b, c);
	float val2 = get_pixel(im, a+1, b, c);
	float val3 = get_pixel(im, a+1, b+1, c);
	float val4 = get_pixel(im, a, b+1, c);
	float ratio_verticalBelow = y - b;
	float ratio_verticalAbove = 1 - ratio_verticalBelow;
	float ratio_horizLeft = x - a;
	float ratio_horizRight = 1 - ratio_horizLeft;
	float val_horizBelow_ip = val2*ratio_horizLeft + val1*ratio_horizRight;
	float val_horizAbove_ip = val3*ratio_horizLeft + val4*ratio_horizRight;  // Interpolated value
	float value = val_horizBelow_ip*ratio_verticalAbove + val_horizAbove_ip*ratio_verticalBelow;
    return value;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
	image resized = make_image(w, h, im.c);
	float width_slope = (float)im.w / w;
	float width_const = 0.5*width_slope - 0.5;
	float height_slope = (float)im.h / h;
	float height_const = 0.5*height_slope - 0.5;
	float orig_width, orig_height;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			for (int k = 0; k < im.c; k++) {
				orig_width = width_slope*i + width_const;
				orig_height = height_slope*j + height_const;
				float value = bilinear_interpolate(im, orig_width, orig_height, k);
				set_pixel(resized, i, j, k, value);
			}
		}
	}
	return resized;
}

