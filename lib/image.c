/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * image.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nadzuna.h"

#include "common.h"
#include "color.h"
#include "error.h"

NADZUNA_API ndz_image_t *
ndz_image_create(int32_t width, int32_t stride, int32_t height, int32_t bpp, ndz_colorfmt_t fmt)
{
	ndz_image_t * img = NULL;
	int32_t bytepp = 0;

	assert(width > 0);
	assert(height > 0);
	assert(bpp > 0);

	if (stride == 0) {
		stride = width;
	}

	if (stride < width) {
		ndz_print_error(__func__, "Stride is smaller than width");
		goto ERR_EXIT;
	}

	bytepp = GetBytesPerPixel_of_Format(fmt);
	if (bytepp < 0) {
		ndz_print_error(__func__, "Unknown color format: %d", fmt);
		goto ERR_EXIT;
	}

	img = malloc(sizeof(ndz_image_t));
	if (img == NULL) {
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	img->pixels = malloc(width * height * bytepp);
	if (img->pixels == NULL) {
		ndz_print_error(__func__, "Failed to allocate memory...");
		free(img);
		img = NULL;
		goto ERR_EXIT;
	}

	img->width  = width;
	img->height = height;
	img->stride = stride;
	img->bpp    = bpp;
	img->fmt    = fmt;

ERR_EXIT:
	return img;
}


NADZUNA_API void
ndz_image_free(ndz_image_t * img)
{
	assert(img != NULL);
	assert(img->pixels != NULL);

	free(img->pixels);
	free(img);
}


NADZUNA_API void
ndz_fill_image(ndz_image_t * img, uint32_t c)
{
	assert(img != NULL);
	assert(img->pixels != NULL);
	assert(img->stride >= img->width);

	int width  = img->width;
	int height = img->height;
	int stride = img->stride;

	int bytepp = GetBytesPerPixel_of_Format(img->fmt);

	switch (bytepp) {
	case 1: {
		uint8_t c8 = (uint8_t)c;
		uint8_t * pixels = (uint8_t *)img->pixels;
		for (int k=0; k<height; k+=1) {
			uint8_t * lp = &(pixels[k * stride]);
			for (int j=0; j<width; j+=1) {
				lp[j] = c8;
			}
		}
		break;
	}

	case 2: {
		uint16_t c16 = (uint16_t)c;
		uint16_t * pixels = (uint16_t *)img->pixels;
		for (int k=0; k<height; k+=1) {
			uint16_t * lp = &(pixels[k * stride]);
			for (int j=0; j<width; j+=1) {
				lp[j] = c16;
			}
		}
		break;
	}

	case 3: {
		uint8_t b0 = ((uint8_t *)&c)[0];
		uint8_t b1 = ((uint8_t *)&c)[1];
		uint8_t b2 = ((uint8_t *)&c)[2];
		uint8_t * pixels = (uint8_t *)img->pixels;
		for (int k=0; k<height; k+=1) {
			uint8_t * lp = &(pixels[k * stride * 3]);
			for (int j=0; j<width; j+=1) {
				lp[j*3+0] = b0;
				lp[j*3+1] = b1;
				lp[j*3+2] = b2;
			}
		}
		break;
	}

	case 4: {
		uint32_t * pixels = (uint32_t *)img->pixels;
		for (int k=0; k<height; k+=1) {
			uint32_t * lp = &(pixels[k * stride]);
			for (int j=0; j<width; j+=1) {
				lp[j] = c;
			}
		}
		break;
	}

	default:
		ndz_print_error(__func__, "Unsupported pixel format: %d", img->fmt);
	}
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
