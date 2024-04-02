/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * jpegtest.c
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

	img = ndz_load_jpeg(argv[1]);
	if (img == NULL) {
		fprintf(stderr, "%s: failed to load image.\n", argv[1]);
		exit(1);
	}
	fprintf(stderr, "load_jpeg(...) = %p\n", img);

	fprintf(stderr, "%s: %dx%d, %dbpp\n", argv[1], img->width, img->height, img->bpp);

	fprintf(stderr, "Output result...\n");
	ndz_save_tga("tmp/output.tga", img);

	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
