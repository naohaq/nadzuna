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
#include "pngfile.h"
#include "error.h"


NADZUNA_API ndz_image_t *
ndz_load_png(const char * filename)
{
	ndz_image_t * dst_img = NULL;
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
	dst_img = ndz_image_create(w, stride, h, bpp, NDZ_COLORFMT_ARGB8888);
	if (dst_img == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	if (png_image_finish_read(&image, NULL, dst_img->pixels, 0, NULL) == 0) {
		ndz_print_error(__func__, "%s", image.message);
		err = 1;
		goto ERR_EXIT;
	}

ERR_EXIT:
	png_image_free(&image);

	if (err) {
		if (dst_img != NULL) {
			ndz_image_free(dst_img);
			dst_img = NULL;
		}
	}
	return dst_img;
}


NADZUNA_API int
ndz_save_png(const char * filename, ndz_image_t * src_img)
{
	int rc = -1;
	int err = 0;

	assert(filename != NULL);
	assert(src_img != NULL);

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	FILE * fp = NULL;

	if (src_img->fmt != NDZ_COLORFMT_ARGB8888 &&
		src_img->fmt != NDZ_COLORFMT_Y8) {
		ndz_print_error(__func__, "Unsupported color format: %d\n", src_img->fmt);
		err = 1;
		goto ERR_EXIT;
	}

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		err = 1;
		goto ERR_EXIT;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		err = 1;
		goto ERR_EXIT;
	}

	png_init_io(png_ptr, fp);

	int w = src_img->width;
	int h = src_img->height;
	int stride = src_img->stride;

	int color_type = 0;
	png_color_8 sig_bit;
	int interlace = PNG_INTERLACE_NONE;

	switch (src_img->fmt) {
	case NDZ_COLORFMT_ARGB8888:
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		sig_bit.red   = 8;
		sig_bit.green = 8;
		sig_bit.blue  = 8;
		sig_bit.alpha = 8;
		break;

	case NDZ_COLORFMT_Y8:
		color_type = PNG_COLOR_TYPE_GRAY;
		sig_bit.gray  = 8;
		break;

	default:
		/* must not be reached. */
		ndz_print_error(__func__, "[BUG] illegal image format: %d", src_img->fmt);
		abort( );
	}

	png_set_IHDR(png_ptr, info_ptr, w, h, 8,
				 color_type, interlace,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	if (src_img->fmt == NDZ_COLORFMT_ARGB8888) {
		png_set_bgr(png_ptr);
	}

	png_set_sBIT(png_ptr, info_ptr, &sig_bit);

	png_set_compression_level(png_ptr, 7);

	png_write_info(png_ptr, info_ptr);
	png_set_shift(png_ptr, &sig_bit);
	png_set_packing(png_ptr);

	switch (src_img->fmt) {
	case NDZ_COLORFMT_ARGB8888: {
		uint32_t * pixels = (uint32_t *)src_img->pixels;
		for (int y=0; y<h; y+=1) {
			uint32_t * slp = &(pixels[stride*y]);
			png_bytep row_ptr = (png_bytep)slp;
			png_write_rows(png_ptr, &row_ptr, 1);
		}
		break;
	}

	case NDZ_COLORFMT_Y8: {
		uint8_t * pixels = (uint8_t *)src_img->pixels;
		for (int y=0; y<h; y+=1) {
			png_bytep row_ptr = (png_bytep)&(pixels[stride*y]);
			png_write_rows(png_ptr, &row_ptr, 1);
		}
		break;
	}

	default:
		/* must not be reached. */
		ndz_print_error(__func__, "[BUG] illegal image format: %d", src_img->fmt);
		abort( );
	}

	rc = 0;

ERR_EXIT:
	if (png_ptr != NULL) {
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	if (err) {
		rc = -1;
	}

	return rc;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
