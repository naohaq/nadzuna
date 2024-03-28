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

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "error.h"

int32_t
GetBytePerPixel_of_Format(ColorFormat_t fmt)
{
	int32_t byte_per_pixel;

	switch (fmt) {
	case COLORFMT_ARGB8888_32: byte_per_pixel = 4; break;
	case COLORFMT_BGRA8888_32: byte_per_pixel = 4; break;
	case COLORFMT_RGB565_16:   byte_per_pixel = 2; break;
	case COLORFMT_ARGB1555_16: byte_per_pixel = 2; break;
	case COLORFMT_YUV444_32:   byte_per_pixel = 4; break;
	case COLORFMT_YUV422_16:   byte_per_pixel = 2; break;
	case COLORFMT_Y8:          byte_per_pixel = 1; break;
	default:                   byte_per_pixel = -1; /* error */
	}

	return byte_per_pixel;
}


yuv_color_t
RGB_to_YUV(uint32_t rgb)
{
	const uint32_t r = (rgb & 0x00ff0000U) >> 16;
	const uint32_t g = (rgb & 0x0000ff00U) >>  8;
	const uint32_t b = (rgb & 0x000000ffU);

	const float32_t fr = (float32_t)r;
	const float32_t fg = (float32_t)g;
	const float32_t fb = (float32_t)b;

	const float32_t fy =  0.299f*fr + 0.587f*fg + 0.114f*fb;
	const float32_t fu = -0.169f*fr - 0.331f*fg + 0.500f*fb;
	const float32_t fv =  0.500f*fr - 0.419f*fg - 0.081f*fb;

	const int32_t y = (int32_t)floorf(fy);
	const int32_t u = (int32_t)floorf(fu);
	const int32_t v = (int32_t)floorf(fv);

	yuv_color_t result;
	result.y = (uint8_t)y;
	result.u = (uint8_t)(u+128);
	result.v = (uint8_t)(v+128);

	return result;
}

uint32_t
YUV_to_RGB(yuv_color_t s)
{
	const float32_t fy = (float32_t)s.y;
	const float32_t fu = (float32_t)((int32_t)s.u - 128);
	const float32_t fv = (float32_t)((int32_t)s.v - 128);

	const float32_t fr = 1.000f*fy - 0.0009267448331229f*fu + 1.4016866683959961f*fv;
	const float32_t fg = 1.000f*fy - 0.3436953723430634f*fu - 0.7141690254211426f*fv;
	const float32_t fb = 1.000f*fy + 1.7721604108810425f*fu + 0.0009902204619721f*fv;

	int32_t r = (int32_t)floorf(fr);
	int32_t g = (int32_t)floorf(fg);
	int32_t b = (int32_t)floorf(fb);
	uint32_t c;

	if (r <   0) r =   0;
	if (r > 255) r = 255;
	if (g <   0) g =   0;
	if (g > 255) g = 255;
	if (b <   0) b =   0;
	if (b > 255) b = 255;

	c = (r << 16) | (g << 8) | b;
	return c;
}


BitmapImage_t *
convert_RGB888_to_Y8(BitmapImage_t * src_img)
{
	BitmapImage_t * dst_img = NULL;
	int32_t k;
	int32_t width;
	int32_t height;
	int32_t stride;
	uint32_t * pixels;

	assert(src_img != NULL);
	assert(src_img->pixels != NULL);

	assert(src_img->fmt == COLORFMT_RGB888_32);

	width  = src_img->width;
	height = src_img->height;
	stride = (src_img->stride != 0) ? src_img->stride : width;
	pixels = (uint32_t *)src_img->pixels;

	dst_img = BitmapImage_Create(width, 0, height, 8, COLORFMT_Y8);
	if (dst_img == NULL) {
		ndz_print_error(__func__, "Memory allocation failed...");
		goto ERR_EXIT;
	}

	for (k=0; k<height; k+=1) {
		int32_t j;
		uint32_t * slp = &(pixels[k * stride]);
		uint8_t  * dlp = &(((uint8_t *)dst_img->pixels)[k * width]);
		for (j=0; j<width; j+=1) {
			yuv_color_t c = RGB_to_YUV(slp[j]);
			dlp[j] = c.y;
		}
	}

ERR_EXIT:
	return dst_img;
}

BitmapImage_t *
convert_Y8_to_RGB888(BitmapImage_t * src_img)
{
	BitmapImage_t * dst_img = NULL;
	int32_t k;
	int32_t width;
	int32_t height;
	int32_t stride;
	uint8_t * pixels;

	assert(src_img != NULL);
	assert(src_img->pixels != NULL);

	assert(src_img->fmt == COLORFMT_Y8);

	width  = src_img->width;
	height = src_img->height;
	stride = (src_img->stride != 0) ? src_img->stride : width;
	pixels = (uint8_t *)src_img->pixels;

	dst_img = BitmapImage_Create(width, 0, height, 32, COLORFMT_RGB888_32);
	if (dst_img == NULL) {
		ndz_print_error(__func__, "Memory allocation failed...");
		goto ERR_EXIT;
	}
	
	for (k=0; k<height; k+=1) {
		int32_t j;
		uint8_t  * slp = &(pixels[k * stride]);
		uint32_t * dlp = &(((uint32_t *)dst_img->pixels)[k * width]);
		for (j=0; j<width; j+=1) {
			yuv_color_t s;
			s.y = slp[j];
			s.u = 128;
			s.v = 128;
			dlp[j] = YUV_to_RGB(s);
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
