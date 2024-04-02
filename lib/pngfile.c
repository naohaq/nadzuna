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

#include "nadzuna.h"

#include "common.h"
#include "color.h"
// #include "file_io.h"
#include "pngfile.h"
#include "error.h"


NADZUNA_API ndz_image_t *
ndz_load_png(const char * filename)
{
	ndz_image_t * result = NULL;
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

	int bytepp = PNG_IMAGE_PIXEL_SIZE(image.format);
	int bpp = bytepp * 8;
	int stride = PNG_IMAGE_ROW_STRIDE(image) / bytepp;
	result = ndz_image_create(w, stride, h, bpp, NDZ_COLORFMT_ARGB8888_32);
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
			ndz_image_free(result);
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
