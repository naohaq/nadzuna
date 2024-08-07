/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * tgatest.c
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "nadzuna.h"

int
main(int argc, char * argv[])
{
	ndz_image_t * img;

	if (argc < 2) {
		exit(1);
	}

	img = ndz_load_tga(argv[1]);
	if (img == NULL) {
		fprintf(stderr, "%s: failed to load image.\n", argv[1]);
		exit(1);
	}
	fprintf(stderr, "load_tga(...) = %p\n", img);

	if (img->fmt == NDZ_COLORFMT_ARGB8888) {
		ndz_image_t * img_y8 = ndz_image_create(img->width, 0, img->height, 8, NDZ_COLORFMT_Y8);

		if (img_y8 != NULL) {
			uint32_t * src_px = (uint32_t *)img->pixels;
			uint8_t * dst_px = (uint8_t *)img_y8->pixels;

			int w = img->width;
			int h = img->height;
			for (int k=0; k<h; k+=1) {
				for (int j=0; j<w; j+=1) {
					uint32_t parity = ((j>>4)&0x01) ^ ((k>>4)&0x01);
					uint32_t bg = parity*0x3f + 0x60;
					uint32_t c  = src_px[k*w+j];
					ndz_yuv_t s = ndz_rgb2yuv(c);
					uint32_t a  = (c & 0xff000000) >> 24;
					uint32_t y  = ((255 - a)*bg + a*s.y) / 255;
					dst_px[k*w+j] = (uint8_t)y;
				}
			}

			fprintf(stderr, "Output result...\n");
			ndz_save_tga("tmp/output.tga", img_y8);
		}
	}
	else if (img->fmt == NDZ_COLORFMT_Y8) {
		fprintf(stderr, "Output result...\n");
		ndz_save_tga("tmp/output.tga", img);
	}
	else {
		fprintf(stderr, "Unknown color format: %d\n", img->fmt);
		exit(1);
	}

	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
