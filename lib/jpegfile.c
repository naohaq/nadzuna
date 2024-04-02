/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * jpegfile.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <setjmp.h>

#include <jpeglib.h>
#include <jerror.h>

#include "nadzuna.h"

#include "common.h"
#include "color.h"
#include "color_inline.h"
#include "error.h"

typedef struct {
	struct jpeg_error_mgr pub;    /* "public" fields */
	jmp_buf setjmp_buffer;        /* for return to caller */
	/* const char * prefix; */
} ndz_jpeg_data_t;

static void
_jpeg_error_exit(j_common_ptr jcp)
{
	ndz_jpeg_data_t * jd = (ndz_jpeg_data_t *)jcp->err;

	jcp->err->output_message(jcp);

	longjmp(jd->setjmp_buffer, 1);
}

static void
_jpeg_emit_message(j_common_ptr jcp, int msg_level)
{
#if 0
	ndz_jpeg_data_t * jd = (ndz_jpeg_data_t *)jcp->err;
#endif

	if (msg_level < 0) {
		jcp->err->output_message(jcp);
	}
}


static struct jpeg_error_mgr *
_jpeg_data_init(ndz_jpeg_data_t * jd)
{
	struct jpeg_error_mgr * jem;

	jem = jpeg_std_error(&(jd->pub));

	jd->pub.error_exit = _jpeg_error_exit;
	jd->pub.emit_message = _jpeg_emit_message;
	/* jd->prefix = "ndz_load_jpeg"; */
	
	return jem;
}


NADZUNA_API ndz_image_t *
ndz_load_jpeg(const char * filename)
{
	int err = 1;
	FILE * fp = NULL;
	ndz_image_t * img = NULL;

	struct jpeg_decompress_struct jds;
	ndz_jpeg_data_t jdata;
	uint8_t * dec_buf = NULL;

	assert(filename != NULL);

	jds.err = _jpeg_data_init(&jdata);
	if (setjmp(jdata.setjmp_buffer)) {
		ndz_print_error(__func__, "hoge!");
		goto ERR_EXIT;
	}
	jpeg_create_decompress(&jds);

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		goto ERR_EXIT;
	}

	jpeg_stdio_src(&jds, fp);

	jpeg_read_header(&jds, TRUE);

	int w = jds.image_width;
	int h = jds.image_height;
	if (! IMG_CHECK_DIMENSIONS(w, h)) {
		ndz_print_error(__func__, "Image size is out of range: %d x %d", w, h);
		goto ERR_EXIT;
	}

	jds.do_fancy_upsampling = FALSE;
	jds.do_block_smoothing = FALSE;
	jpeg_start_decompress(&jds);

	if ((jds.rec_outbuf_height > 16) || (jds.output_components <= 0)) {
		ndz_print_error(__func__, "Unsupported format.");
		goto ERR_EXIT;
	}

	ndz_colorfmt_t fmt = NDZ_COLORFMT_ARGB8888_32;
	if (jds.out_color_space == JCS_RGB) {
		fmt = NDZ_COLORFMT_ARGB8888_32;
	}
	else if (jds.out_color_space == JCS_GRAYSCALE) {
		fmt = NDZ_COLORFMT_Y8;
	}
	else {
		ndz_print_error(__func__, "Unsupported color space.");
		goto ERR_EXIT;
	}

	int outbuf_height = jds.rec_outbuf_height;
	int src_bytepp = jds.output_components;

	int dst_bytepp = GetBytePerPixel_of_Format(fmt);
	img = ndz_image_create(w, 0, h, dst_bytepp*8, fmt);
	if (img == NULL) {
		goto ERR_EXIT;
	}

	dec_buf = malloc(w * 16 * src_bytepp);
	if (dec_buf == NULL) {
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	uint8_t * lines[16];
	for (int k=0; k < outbuf_height; k+=1) {
		lines[k] = &(dec_buf[k * w * src_bytepp]);
	}

	for (int y=0; y < h; y += outbuf_height) {
		jpeg_read_scanlines(&jds, lines, outbuf_height);

		int scans = outbuf_height;
		if ((h - y) < outbuf_height) {
			scans = h - y;
		}

		for (int k=0; k < scans; k+=1) {
			uint8_t * src_lp = lines[k];

			switch (jds.out_color_space) {
			case JCS_RGB: {
				uint32_t * pixels = (uint32_t *)img->pixels;
				uint32_t * dst_lp = &(pixels[(y+k)*w]);
				for (int x=0; x<w; x+=1) {
					uint8_t r = src_lp[x*src_bytepp + 0];
					uint8_t g = src_lp[x*src_bytepp + 1];
					uint8_t b = src_lp[x*src_bytepp + 2];
					dst_lp[x] = combine_ARGB(0xff,r,g,b);
				}
				break;
			}

			case JCS_GRAYSCALE: {
				uint8_t * pixels = (uint8_t *)img->pixels;
				uint8_t * dst_lp = &(pixels[(y+k)*w]);
				for (int x=0; x<w; x+=1) {
					dst_lp[x] = src_lp[x];
				}
				break;
			}

			default:
				/* must not be reached. */
				ndz_print_error(__func__, "[BUG] unsupported color space: %d", jds.out_color_space);
				abort( );
			}
		}
	}

	err = 0;

ERR_EXIT:
	jpeg_destroy_decompress(&jds);

	if (dec_buf != NULL) {
		free(dec_buf);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	if (err) {
		if (img != NULL) {
			ndz_image_free(img);
			img = NULL;
		}
	}
	return img;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
