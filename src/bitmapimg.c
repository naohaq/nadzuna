/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * bitmapimg.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "error.h"

BitmapImage_t *
BitmapImage_Create(int32_t width, int32_t stride, int32_t height, int32_t bpp, ColorFormat_t fmt)
{
	BitmapImage_t * img = NULL;
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

	bytepp = GetBytePerPixel_of_Format(fmt);
	if (bytepp < 0) {
		ndz_print_error(__func__, "Unknown color format: %d", fmt);
		goto ERR_EXIT;
	}

	img = malloc(sizeof(BitmapImage_t));
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
	img->align  = 0;

ERR_EXIT:
	return img;
}


void
BitmapImage_Free(BitmapImage_t * img)
{
	assert(img != NULL);
	assert(img->pixels != NULL);

	free(img->pixels);
	free(img);
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
