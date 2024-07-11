/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * color.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nadzuna.h"

#include "common.h"
#include "color.h"
#include "color_inline.h"
#include "error.h"

static inline uint8_t
clampU8(int32_t x)
{
	uint8_t y;
	if (x < 0) {
		y = 0;
	}
	else if (x > 255) {
		y = 255;
	}
	else {
		y = (uint8_t)x;
	}
	return y;
}

static inline void
decomp_rgb(uint32_t rgb, uint32_t * rp, uint32_t * gp, uint32_t * bp)
{
	*rp = (rgb & 0x00ff0000U) >> 16;
	*gp = (rgb & 0x0000ff00U) >>  8;
	*bp = (rgb & 0x000000ffU);
}

static inline double
rgb_to_y(double fr, double fg, double fb)
{
	return  0.299*fr + 0.587*fg + 0.114*fb;
}

static inline double
rgb_to_u(double fr, double fg, double fb)
{
	return -0.169*fr - 0.331*fg + 0.500*fb;
}

static inline double
rgb_to_v(double fr, double fg, double fb)
{
	return  0.500*fr - 0.419*fg - 0.081*fb;
}

static inline double
yuv_to_r(double fy, double fu, double fv)
{
	return 1.000*fy - 0.0009267448331229*fu + 1.4016866683959961*fv;
}

static inline double
yuv_to_g(double fy, double fu, double fv)
{
	return 1.000*fy - 0.3436953723430634*fu - 0.7141690254211426*fv;
}

static inline double
yuv_to_b(double fy, double fu, double fv)
{
	return 1.000*fy + 1.7721604108810425*fu + 0.0009902204619721*fv;
}


int32_t
GetBytesPerPixel_of_Format(ndz_colorfmt_t fmt)
{
	int32_t bytes_per_pixel;

	switch (fmt) {
	case NDZ_COLORFMT_ARGB8888:   bytes_per_pixel = 4; break;
	case NDZ_COLORFMT_RGB888:     bytes_per_pixel = 3; break;
	case NDZ_COLORFMT_RGB565:     bytes_per_pixel = 2; break;
	case NDZ_COLORFMT_ARGB1555:   bytes_per_pixel = 2; break;
	case NDZ_COLORFMT_Y8:         bytes_per_pixel = 1; break;
	default:                      bytes_per_pixel = -1; /* error */
	}

	return bytes_per_pixel;
}

NADZUNA_API int
ndz_bytes_per_pixel(ndz_colorfmt_t fmt)
{
	return GetBytesPerPixel_of_Format(fmt);
}


NADZUNA_API ndz_yuv_t
ndz_rgb2yuv(uint32_t rgb)
{
	uint32_t r;
	uint32_t g;
	uint32_t b;

	decomp_rgb(rgb, &r, &g, &b);

	const double fr = (double)r;
	const double fg = (double)g;
	const double fb = (double)b;

	const double fy = rgb_to_y(fr, fg, fb);
	const double fu = rgb_to_u(fr, fg, fb);
	const double fv = rgb_to_v(fr, fg, fb);

	const int32_t y = (int32_t)floor(fy);
	const int32_t u = (int32_t)floor(fu);
	const int32_t v = (int32_t)floor(fv);

	ndz_yuv_t result;
	result.y = (uint8_t)y;
	result.u = (uint8_t)(u+128);
	result.v = (uint8_t)(v+128);

	return result;
}

NADZUNA_API uint32_t
ndz_yuv2rgb(ndz_yuv_t s)
{
	const double fy = (double)s.y;
	const double fu = (double)((int32_t)s.u - 128);
	const double fv = (double)((int32_t)s.v - 128);

	const double fr = yuv_to_r(fy, fu, fv);
	const double fg = yuv_to_g(fy, fu, fv);
	const double fb = yuv_to_b(fy, fu, fv);

	int32_t r = (int32_t)floor(fr);
	int32_t g = (int32_t)floor(fg);
	int32_t b = (int32_t)floor(fb);
	uint32_t c = combine_ARGB(0, clampU8(r), clampU8(g), clampU8(b));

	return c;
}


ndz_image_t *
convert_RGB_to_Y8(ndz_image_t * src_img)
{
	ndz_image_t * dst_img = NULL;
	int32_t k;
	int32_t width;
	int32_t height;
	int32_t stride;
	uint32_t * pixels;

	assert(src_img != NULL);
	assert(src_img->pixels != NULL);

	assert(src_img->fmt == NDZ_COLORFMT_ARGB8888);

	width  = src_img->width;
	height = src_img->height;
	stride = (src_img->stride != 0) ? src_img->stride : width;
	pixels = (uint32_t *)src_img->pixels;

	dst_img = ndz_image_create(width, 0, height, 8, NDZ_COLORFMT_Y8);
	if (dst_img == NULL) {
		ndz_print_error(__func__, "Memory allocation failed...");
		goto ERR_EXIT;
	}

	for (k=0; k<height; k+=1) {
		int32_t j;
		uint32_t * slp = &(pixels[k * stride]);
		uint8_t  * dlp = &(((uint8_t *)dst_img->pixels)[k * width]);
		for (j=0; j<width; j+=1) {
			uint32_t r;
			uint32_t g;
			uint32_t b;
			decomp_rgb(slp[j], &r, &g, &b);
			double fy = rgb_to_y((double)r, (double)g, (double)b);
			int32_t y = (int32_t)floor(fy);
			dlp[j] = (uint8_t)y;
		}
	}

ERR_EXIT:
	return dst_img;
}

ndz_image_t *
convert_Y8_to_RGB(ndz_image_t * src_img)
{
	ndz_image_t * dst_img = NULL;
	int32_t k;
	int32_t width;
	int32_t height;
	int32_t stride;
	uint8_t * pixels;

	assert(src_img != NULL);
	assert(src_img->pixels != NULL);

	assert(src_img->fmt == NDZ_COLORFMT_Y8);

	width  = src_img->width;
	height = src_img->height;
	stride = (src_img->stride != 0) ? src_img->stride : width;
	pixels = (uint8_t *)src_img->pixels;

	dst_img = ndz_image_create(width, 0, height, 32, NDZ_COLORFMT_ARGB8888);
	if (dst_img == NULL) {
		ndz_print_error(__func__, "Memory allocation failed...");
		goto ERR_EXIT;
	}
	
	for (k=0; k<height; k+=1) {
		int32_t j;
		uint8_t  * slp = &(pixels[k * stride]);
		uint32_t * dlp = &(((uint32_t *)dst_img->pixels)[k * width]);
		for (j=0; j<width; j+=1) {
			uint8_t y = slp[j];
			dlp[j] = combine_ARGB(255, y, y, y);
		}
	}

ERR_EXIT:
	return dst_img;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
