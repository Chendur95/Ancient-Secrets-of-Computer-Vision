#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int check_limits(int val, int start, int end) {
	if (val <= start) {
		val = start;
	}
	else if (val >= end) {
		val = end - 1;
	}
	return val;
}

float get_pixel(image im, int x, int y, int c)
{
	// TODO Fill this in
	x = check_limits(x, 0, im.w);
	y = check_limits(y, 0, im.h);
	c = check_limits(c, 0, im.c);
	return im.data[x + im.w*y + im.h*im.w*c];
}

void set_pixel(image im, int x, int y, int c, float v)
{
	// TODO Fill this in
	im.data[x + im.w*y + im.h*im.w*c] = v;
}

image copy_image(image im)
{
	image copy = make_image(im.w, im.h, im.c);
	// TODO Fill this in
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			for (int k = 0; k < im.c; k++) {
				float val = get_pixel(im, i, j, k);
				set_pixel(copy, i, j, k, val);
			}
		}
	}
	return copy;
}

image rgb_to_grayscale(image im)
{
	assert(im.c == 3);
	image gray = make_image(im.w, im.h, 1);
	// TODO Fill this in
	float val = 0;
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			val = 0.299*get_pixel(im, i, j, 0) + 0.587*get_pixel(im, i, j, 1) + 0.114*get_pixel(im, i, j, 2);
			set_pixel(gray, i, j, 0, val);
		}
	}
	return gray;
}

void shift_image(image im, int c, float v)
{
	// TODO Fill this in
	float val = 0;
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			val = get_pixel(im, i, j, c) + v;
			set_pixel(im, i, j, c, val);
		}
	}
}

void clamp_image(image im)
{
	// TODO Fill this in
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			for (int k = 0; k < im.c; k++) {
				if (get_pixel(im, i, j, k) <= 0)
					set_pixel(im, i, j, k, 0);
				else if (get_pixel(im, i, j, k) >= 1)
					set_pixel(im, i, j, k, 1);
			}
		}
	}
}

// These might be handy
float three_way_max(float a, float b, float c)
{
	return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
	return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im)
{
	// TODO Fill this in
	for (int i = 0; i < im.w; i++) {
		for (int j = 0; j < im.h; j++) {
			float r_comp = get_pixel(im, i, j, 0);
			float g_comp = get_pixel(im, i, j, 1);
			float b_comp = get_pixel(im, i, j, 2);
			float value = three_way_max(r_comp, g_comp, b_comp);
			set_pixel(im, i, j, 2, value);
			float minimum = three_way_min(r_comp, g_comp, b_comp);
			float saturation;
			float C = value - minimum;
			if (value == 0) {
				saturation = 0;
			}
			else {
				saturation = C / value;
			}
			set_pixel(im, i, j, 1, saturation);
			float h_dash = 0, hue;
			if (C == 0) {
				hue = 0;
			}
			else if (value == r_comp) {
				h_dash = (g_comp - b_comp) / C;
			}
			else if (value == g_comp) {
				h_dash = 2 + (b_comp - r_comp) / C;
			}
			else if (value == b_comp) {
				h_dash = 4 + (r_comp - g_comp) / C;
			}
			if (h_dash < 0) {
				hue = 1 + h_dash / 6;
			}
			else {
				hue = h_dash / 6;
			}
			set_pixel(im, i, j, 0, hue);
		}
	}
}

void hsv_to_rgb(image im)
{
	// TODO Fill this in
	float r, g, b;
	for (int i = 0; i<im.w; i++) {
		for (int j = 0; j<im.h; j++) {
			float h = get_pixel(im, i, j, 0);
			float s = get_pixel(im, i, j, 1);
			float v = get_pixel(im, i, j, 2);

			float C = s * v;
			float min = v - C;
			float h_dash = 6 * h;
			float ratio = fmod(h_dash, 2); // Difference ratio without the added constant
			float with_mid = fabs(ratio - 1);
			float midMinusMin_by_C = 1 - with_mid; // Ratio of difference between mid and min value to C
			float mid_value = min + C*midMinusMin_by_C; // Middle among r,g,b

			if (0 <= h_dash && h_dash <= 1) {
				r = C + min;
				g = mid_value;
				b = min;
			}
			else if (1 <= h_dash && h_dash <= 2) {
				r = mid_value;
				g = C + min;
				b = min;
			}
			else if (2 <= h_dash && h_dash <= 3) {
				r = min;
				g = C + min;
				b = mid_value;
			}
			else if (3 <= h_dash && h_dash <= 4) {
				r = min;
				g = mid_value;
				b = C + min;
			}
			else if (4 <= h_dash && h_dash <= 5) {
				r = mid_value;
				g = min;
				b = C + min;
			}
			else if (5 <= h_dash && h_dash <= 6) {
				r = C + min;
				g = min;
				b = mid_value;
			}
			else {
				r = min;
				g = min;
				b = min;
			}
			set_pixel(im, i, j, 0, r);
			set_pixel(im, i, j, 1, g);
			set_pixel(im, i, j, 2, b);
		}
	}
}
