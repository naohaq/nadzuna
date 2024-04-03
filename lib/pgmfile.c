/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * pgmfile.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "nadzuna.h"

#include "common.h"
#include "color.h"
#include "error.h"

NADZUNA_API ndz_image_t *
ndz_load_pgm(const char * filename)
{
	ndz_image_t * img = NULL;
	FILE * fp;
	int err = 1;
	char buf[80];

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		goto ERR_EXIT;
	}

	if (fgets(buf, 80, fp) != NULL) {
		if ((buf[0] != 'P') || (buf[1] != '5')) {
			ndz_print_error(__func__, "%s: Malformed input file...", filename);
			goto ERR_EXIT;
		}
	}
	else {
		if (feof(fp)) {
			ndz_print_error(__func__, "%s: Unexpected EOF...", filename);
		}
		else {
			ndz_print_strerror(__func__, "fgets");
		}
		goto ERR_EXIT;
	}

	int w;
	int h;
	if (fgets(buf, 80, fp) != NULL) {
		int ret;
		ret = sscanf(buf, "%d %d", &w, &h);
		if (ret != 2) {
			ndz_print_error(__func__, "%s: Malformed input file...", filename);
			goto ERR_EXIT;
		}
	}
	else {
		if (feof(fp)) {
			ndz_print_error(__func__, "%s: Unexpected EOF...", filename);
		}
		else {
			ndz_print_strerror(__func__, "fgets");
		}
		goto ERR_EXIT;
	}

	if (fgets(buf, 80, fp) != NULL) {
		if ((buf[0] != '2') || (buf[1] != '5') || (buf[2] != '5')) {
			ndz_print_error(__func__, "%s: Malformed input file...", filename);
			goto ERR_EXIT;
		}
	}
	else {
		if (feof(fp)) {
			ndz_print_error(__func__, "%s: Unexpected EOF...", filename);
		}
		else {
			ndz_print_strerror(__func__, "fgets");
		}
		goto ERR_EXIT;
	}

	if (! IMG_CHECK_DIMENSIONS(w, h)) {
		ndz_print_error(__func__, "Image size is out of range: %d x %d", w, h);
		goto ERR_EXIT;
	}

	int32_t stride = (w + 3) & 0x7ffffffc;
	img = ndz_image_create(w, stride, h, 8, NDZ_COLORFMT_Y8);
	if (img == NULL) {
		goto ERR_EXIT;
	}

	err = 0;
	for (int k=0; k<h; k+=1) {
		int32_t ret;

		ret = fread(&(((uint8_t *)img->pixels)[k * stride]), 1, w, fp);
		if (ret < w) {
			if (feof(fp)) {
				ndz_print_error(__func__, "%s: Unexpected EOF...", filename);
			}
			else {
				ndz_print_strerror(__func__, "fread");
			}
			err = 1;
			break;
		}
	}

ERR_EXIT:
	if (err) {
		if (img != NULL) {
			free(img->pixels);
			free(img);
			img = NULL;
		}
	}
	if (fp != NULL) {
		fclose(fp);
	}
	return img;
}


NADZUNA_API int
ndz_save_pgm(const char * filename, ndz_image_t * img)
{
	int ret = 0;
	uint8_t * out_buf = NULL;
	FILE * out_fp = NULL;

	assert(img != NULL);
	assert(filename != NULL);

	switch (img->fmt) {
	case NDZ_COLORFMT_Y8:
		break;

	case NDZ_COLORFMT_ARGB8888:
		break;

	default:
		ndz_print_error(__func__, "Unsupported color format.");
		ret = -1;
		goto ERR_EXIT;
	}

	int width  = img->width;
	int height = img->height;
	int stride = (img->stride == 0) ? img->width : img->stride;

	out_fp = fopen(filename, "wb");
	if (out_fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		ret = -1;
		goto ERR_EXIT;
	}

	fprintf(out_fp, "P5\n");
	fprintf(out_fp, "%d %d\n", width, height);
	fprintf(out_fp, "255\n");

	if (img->fmt == NDZ_COLORFMT_Y8) {
		uint8_t * pixels = (uint8_t *)img->pixels;

		for (int k=0; k<height; k+=1) {
			uint8_t * out_ln = &(pixels[k * stride]);
			fwrite(out_ln, 1, width, out_fp);
		}
	}
	else if (img->fmt == NDZ_COLORFMT_ARGB8888) {
		uint32_t * pixels = (uint32_t *)img->pixels;

		out_buf = malloc(width);
		if (out_buf == NULL) {
			ndz_print_error(__func__, "Failed to allocate memory...");
			ret = -1;
			goto ERR_EXIT;
		}

		for (int k=0; k<height; k+=1) {
			uint32_t * slp = &(pixels[k * stride]);

			for (int j=0; j<width; j+=1) {
				uint32_t c = slp[j];
				ndz_yuv_t s = ndz_rgb2yuv(c);
				out_buf[j] = s.y;
			}

			fwrite(out_buf, 1, width, out_fp);
		}
	}

ERR_EXIT:
	if (out_buf != NULL) {
		free(out_buf);
	}
	if (out_fp != NULL) {
		fclose(out_fp);
	}
	return ret;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
