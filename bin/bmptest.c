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

#include "nadzuna.h"

#include "common.h"

int
main(int argc, char * argv[])
{
	ndz_image_t * img;

	if (argc < 2) {
		exit(1);
	}

	img = ndz_load_bmp(argv[1]);
	if (img == NULL) {
		fprintf(stderr, "%s: failed to load image.\n", argv[1]);
		exit(1);
	}
	fprintf(stderr, "load_bmp(...) = %p\n", img);

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
