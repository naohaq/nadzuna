/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * pngfile.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <png.h>

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "file_io.h"
#include "pngfile.h"
#include "error.h"


BitmapImage_t *
load_png(const char * filename)
{
	BitmapImage_t * result = NULL;
	int err = 0;
	png_image image;

	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename) == 0) {
		ndz_print_error(__func__, "%s", image.message);
		err = 1;
		goto ERR_EXIT;
	}

	image.format = PNG_FORMAT_BGRA;

	int w = image.width;
	int h = image.height;
	if (! IMG_CHECK_DIMENSIONS(w, h)) {
		ndz_print_error(__func__, "Image size is out of range: %d x %d", w, h);
		err = 1;
		goto ERR_EXIT;
	}

	fprintf(stderr, "%s: %dx%d %dbpp\n", __func__, w, h, PNG_IMAGE_PIXEL_CHANNELS(image.format)*8);

	int bpp = 32;
	int bytepp = (bpp + 7) >> 3;
	int stride = PNG_IMAGE_ROW_STRIDE(image) / bytepp;
	result = BitmapImage_Create(w, stride, h, bytepp, COLORFMT_ARGB8888_32);
	if (result == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	if (png_image_finish_read(&image, NULL, result->pixels, 0, NULL) == 0) {
		ndz_print_error(__func__, "%s", image.message);
		err = 1;
		goto ERR_EXIT;
	}

ERR_EXIT:
	png_image_free(&image);

	if (err) {
		if (result != NULL) {
			BitmapImage_Free(result);
			result = NULL;
		}
	}
	return result;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
