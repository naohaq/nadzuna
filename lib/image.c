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


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
