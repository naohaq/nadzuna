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

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "tgafile.h"
#include "pgmfile.h"

int
main(int argc, char * argv[])
{
	BitmapImage_t * img;

	if (argc < 2) {
		exit(1);
	}

	img = load_tga(argv[1]);
	if (img == NULL) {
		fprintf(stderr, "%s: failed to load image.\n", argv[1]);
		exit(1);
	}
	fprintf(stderr, "load_tga(...) = %p\n", img);

	if (img->fmt == COLORFMT_ARGB8888_32) {
		BitmapImage_t * img_y8 = BitmapImage_Create(img->width, 0, img->height, 8, COLORFMT_Y8);

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
					yuv_color_t s = RGB_to_YUV(c);
					uint32_t a  = (c & 0xff000000) >> 24;
					uint32_t y  = ((255 - a)*bg + a*s.y) / 255;
					dst_px[k*w+j] = (uint8_t)y;
				}
			}

			fprintf(stderr, "Output result...\n");
			save_tga("tmp/output.tga", img_y8);
		}
	}
	else if (img->fmt == COLORFMT_Y8) {
		fprintf(stderr, "Output result...\n");
		save_tga("tmp/output.tga", img);
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
