/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * bmptest.c
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
#include "bmpfile.h"
#include "tgafile.h"

int
main(int argc, char * argv[])
{
	int ret;
	BitmapImage_t * img;

	if (argc < 2) {
		exit(1);
	}

	img = load_bmp(argv[1]);
	if (img == NULL) {
		fprintf(stderr, "%s: failed to load image.\n", argv[1]);
		exit(1);
	}
	fprintf(stderr, "load_bmp(...) = %p\n", img);

	fprintf(stderr, "Output result...\n");
	save_tga("tmp/output.tga", img);

	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
