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


int
main(int argc, char * argv[])
{
	BitmapImage_t * img;

	if (argc < 2) {
		exit(1);
	}

	img = load_tga(argv[1]);
	printf("load_tga: %p\n", img);

	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
