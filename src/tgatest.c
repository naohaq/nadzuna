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
		exit(1);
	}
	fprintf(stderr, "load_tga(...) = %p\n", img);

	BitmapImage_t * img_y8 = BitmapImage_Create(img->width, 0, img->height, 8, COLORFMT_Y8);

	if (img_y8 != NULL) {
		uint32_t * src_px = (uint32_t *)img->pixels;
		uint8_t * dst_px = (uint8_t *)img_y8->pixels;

		for (int k=0; k<(img->width*img->height); k+=1) {
			uint32_t c = src_px[k];
			yuv444_color_t s = RGB_to_YUV(c);
			uint32_t a = (c & 0xff000000) >> 24;
			uint32_t y = (a * s.y) / 255;
			dst_px[k] = (uint8_t)y;
		}

		fprintf(stderr, "Output result...\n");
		save_pgm("tmp/output.pgm", img_y8);
	}

	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
