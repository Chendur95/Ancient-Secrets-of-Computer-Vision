#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void l1_normalize(image im)
{
    // TODO
	for (int i = 0; i < im.c; i++) {
		float normalising_factor = 0;
		for (int j = 0; j < im.w; j++) {
			for (int k = 0; k < im.h; k++) {
				normalising_factor += get_pixel(im, j, k, i);
			}
		}
		for (int j = 0; j < im.w; j++) {
			for (int k = 0; k < im.h; k++) {
				float value = get_pixel(im, j, k, i);
				set_pixel(im, j, k, i, value / normalising_factor);
			}
		}
	}
}

image make_box_filter(int w)
{
    // TODO
	image filter = make_image(w, w, 1);
	float normaliser = w*w;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < w; j++) {
			set_pixel(filter, i, j, 0, 1 / normaliser);
		}
	}
    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
	int assertion = 0;
	int channelResult = 1;
	int repeatFilter;
	if (filter.c == im.c || filter.c == 1) assertion = 1;
	assert(assertion==1);
	if (filter.c == im.c) repeatFilter = 0;
	else repeatFilter = 1;
	if (preserve == 1) channelResult = im.c;

	image convolve = make_image(im.w, im.h, channelResult);
	float pixel_value = 0;

	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			pixel_value = 0;
			for (int k = 0; k < im.c; k++) {
				int fX = i - filter.w / 2;
				int fY = j - filter.h / 2;
				for (int a = 0; a < filter.w; a++) {
					for (int b = 0; b < filter.h; b++) {
						if(!repeatFilter)
							pixel_value += get_pixel(filter, a, b, k)*get_pixel(im, fX + a, fY + b, k);
						else
							pixel_value += get_pixel(filter, a, b, 0)*get_pixel(im, fX + a, fY + b, k);
					}
				}
				if (channelResult > 1) { 
					set_pixel(convolve, i, j, k, pixel_value);
					pixel_value = 0;
				}
			}
			if (channelResult == 1) set_pixel(convolve, i, j, 0, pixel_value);
		}
	}
    return convolve;
}

image make_highpass_filter()
{
    // TODO
	image filter = make_image(3, 3, 1);
	set_pixel(filter, 0, 0, 0, 0);
	set_pixel(filter, 0, 1, 0, -1);
	set_pixel(filter, 0, 2, 0, 0);
	set_pixel(filter, 1, 0, 0, -1);
	set_pixel(filter, 1, 1, 0, 4);
	set_pixel(filter, 1, 2, 0, -1);
	set_pixel(filter, 2, 0, 0, 0);
	set_pixel(filter, 2, 1, 0, -1);
	set_pixel(filter, 2, 2, 0, 0);
	return filter;
}

image make_sharpen_filter()
{
    // TODO
	image filter = make_image(3,3,1);
	set_pixel(filter, 0, 0, 0, 0);
	set_pixel(filter, 0, 1, 0, -1);
	set_pixel(filter, 0, 2, 0, 0);
	set_pixel(filter, 1, 0, 0, -1);
	set_pixel(filter, 1, 1, 0, 5);
	set_pixel(filter, 1, 2, 0, -1);
	set_pixel(filter, 2, 0, 0, 0);
	set_pixel(filter, 2, 1, 0, -1);
	set_pixel(filter, 2, 2, 0, 0);
	return filter;
}

image make_emboss_filter()
{
    // TODO
	image filter = make_image(3, 3, 1);
	set_pixel(filter, 0, 0, 0, -2);
	set_pixel(filter, 0, 1, 0, -1);
	set_pixel(filter, 0, 2, 0, 0);
	set_pixel(filter, 1, 0, 0, -1);
	set_pixel(filter, 1, 1, 0, 1);
	set_pixel(filter, 1, 2, 0, 1);
	set_pixel(filter, 2, 0, 0, 0);
	set_pixel(filter, 2, 1, 0, 1);
	set_pixel(filter, 2, 2, 0, 2);
	return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Sharpen, Emboss

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

float compute_gaussian(float sigma, int x, int y, int center) {
	float X = center - x;
	float Y = center - y;
	float factor = 1/(TWOPI*sigma*sigma);
	float gaussian = factor*exp(-(X*X + Y*Y) / (2 * sigma*sigma));
	return gaussian;
}

image make_gaussian_filter(float sigma)
{
    // TODO
	int kernelSize = ceil(6 * sigma);
	if (fmod(kernelSize, 2) == 0) kernelSize++;
	image gaussian_filter = make_image(kernelSize, kernelSize, 1);
	int center = kernelSize / 2;
	float value = 0; 
	for (int i = 0; i < kernelSize; i++) {
		for (int j = 0; j < kernelSize; j++) {
			value = compute_gaussian(sigma, i, j, center);
			set_pixel(gaussian_filter, i, j, 0, value);
		}
	}
	l1_normalize(gaussian_filter);
    return gaussian_filter;
}

image add_image(image a, image b)
{
    // TODO
	assert(a.w == b.w && a.h == b.h && a.c == b.c);

	image added = make_image(a.w, a.h, a.c);
	for (int i = 0; i < a.w; i++) {
		for (int j = 0; j < a.h; j++) {
			for (int k = 0; k < a.c; k++) {
				float val = get_pixel(a, i, j, k) + get_pixel(b, i, j, k);
				set_pixel(added, i, j, k, val);
			}
		}
	}
    return added;
}

image sub_image(image a, image b)
{
    // TODO
	assert(a.w == b.w && a.h == b.h && a.c == b.c);

	image sub = make_image(a.w, a.h, a.c);
	for (int i = 0; i < a.w; i++) {
		for (int j = 0; j < a.h; j++) {
			for (int k = 0; k < a.c; k++) {
				float val = get_pixel(a, i, j, k) - get_pixel(b, i, j, k);
				set_pixel(sub, i, j, k, val);
			}
		}
	}
	return sub;
}

image make_gx_filter()
{
    // TODO
	image filter = make_image(3, 3, 1);
	set_pixel(filter, 0, 0, 0, -1);
	set_pixel(filter, 0, 1, 0, -2);
	set_pixel(filter, 0, 2, 0, -1);

	set_pixel(filter, 1, 0, 0, 0);
	set_pixel(filter, 1, 1, 0, 0);
	set_pixel(filter, 1, 2, 0, 0);

	set_pixel(filter, 2, 0, 0, 1);
	set_pixel(filter, 2, 1, 0, 2);
	set_pixel(filter, 2, 2, 0, 1);

	return filter;
}

image make_gy_filter()
{
    // TODO
	image filter = make_image(3, 3, 1);
	set_pixel(filter, 0, 0, 0, -1);
	set_pixel(filter, 0, 1, 0, 0);
	set_pixel(filter, 0, 2, 0, 1);

	set_pixel(filter, 1, 0, 0, -2);
	set_pixel(filter, 1, 1, 0, 0);
	set_pixel(filter, 1, 2, 0, 2);

	set_pixel(filter, 2, 0, 0, -1);
	set_pixel(filter, 2, 1, 0, 0);
	set_pixel(filter, 2, 2, 0, 1);

	return filter;
}

void feature_normalize(image im)
{
    // TODO
	float pixelValue, normValue;
	float maxima = INT_MIN;
	float minima = INT_MAX;
	for (int i = 0; i < im.w*im.h*im.c; i++) {
		pixelValue = im.data[i];
		maxima = MAX(maxima, pixelValue);
		minima = MIN(minima, pixelValue);
	}
	float range = maxima - minima;
	
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			for (int k = 0; k < im.c; k++) {
				pixelValue = get_pixel(im, i, j, k);
				if (range != 0) normValue = (pixelValue - minima) / range;
				else normValue = 0;
				set_pixel(im, i, j, k, normValue);
			}
		}
	}
}

image *sobel_image(image im)
{
    // TODO
	image *images = calloc(2, sizeof(image));
	images[0] = make_image(im.w, im.h, 1);
	images[1] = make_image(im.w, im.h, 1);
	image Gx_filter = make_gx_filter();
	image Gy_filter = make_gy_filter();
	image Gx = convolve_image(im, Gx_filter, 0);
	image Gy = convolve_image(im, Gy_filter, 0);
	float pix1, pix2;
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			pix1 = get_pixel(Gx, i, j, 0);
			pix2 = get_pixel(Gy, i, j, 0);
			float mag = sqrt(pix1*pix1 + pix2*pix2);
			float direction = atan2(pix2, pix1);
			set_pixel(images[0], i, j, 0, mag);
			set_pixel(images[1], i, j, 0, direction);
		}
	}
	return images;
}

image colorize_sobel(image im)
{
    // TODO
	image *images = sobel(im);
	image magnitude = images[0];
	image direction = images[1];
	feature_normalize(magnitude);
	feature_normalize(direction);
	float hue, sv;
	image colorize_sobel = make_image(im.w, im.h, 3);
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			for (int k = 0; k < 1; k++) {
				hue = get_pixel(magnitude, i, j, k);
				sv = get_pixel(direction, i, j, k);
				set_pixel(colorize_sobel, i, j, 0, hue);
				set_pixel(colorize_sobel, i, j, 1, sv);
				set_pixel(colorize_sobel, i, j, 2, sv);
			}
		}
	}
	return colorize_sobel;
}
