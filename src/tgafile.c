/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * tgafile.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "file_io.h"
#include "tgafile.h"
#include "error.h"

struct ST_TGA_HEADER {
	uint8_t id_len;
	uint8_t cmap_type;
	uint8_t img_type;
	int16_t cmap_org;
	int16_t cmap_len;
	uint8_t cmap_size;
	int16_t img_org_x;
	int16_t img_org_y;
	int16_t img_width;
	int16_t img_height;
	uint8_t img_bpp;
	uint8_t img_desc;
	uint8_t img_id[];
};

typedef struct ST_TGA_HEADER TGAHeader_t;

static int load_tga_header(FILE * fp, TGAHeader_t * hdr);

#define FREAD_CHECK_ERROR(x) do {						\
		if ((x) == 0) {									\
			ndz_print_error(__func__, "Unexpected EOF"); \
			ret_val = -1;								\
			goto ERR_EXIT;								\
		}												\
		else if ((x) == -1) {							\
			ndz_print_strerror(__func__, "fread");		\
			ret_val = -1;								\
			goto ERR_EXIT;								\
		}												\
	} while(0)

#define FWRITE_CHECK_ERROR(x) do {					\
		if ((x) < 0) {								\
			ndz_print_strerror(__func__, "fwrite");	\
			ret_val = -1;							\
			goto ERR_EXIT;							\
		}											\
	} while (0)										\


static const char *
get_image_type_string(int img_type)
{
	const char * str = NULL;

	switch (img_type) {
	case  0: str = "No image data included";  break;
	case  1: str = "Color-mapped image";      break;
	case  2: str = "Full color image";        break;
	case  3: str = "Grayscale image";         break;
	case  9: str = "RLE, Color-mapped image"; break;
	case 10: str = "RLE, Full color image";   break;
	case 11: str = "RLE, Grayscale image";    break;
	default:
		/* Unknown color type */
		str = NULL;
	}

	return str;
}

static int
load_tga_header(FILE * fp, TGAHeader_t * hdr)
{
	int32_t ret;
	int32_t ret_val = 0;

	assert(hdr != NULL);

	ret = ndz_fread_U8str(fp, &(hdr->id_len), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(hdr->cmap_type), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(hdr->img_type), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->cmap_org));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->cmap_len));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(hdr->cmap_size), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->img_org_x));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->img_org_y));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->img_width));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(hdr->img_height));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(hdr->img_bpp), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(hdr->img_desc), 1);
	FREAD_CHECK_ERROR(ret);

ERR_EXIT:
	return ret_val;
}


BitmapImage_t *
load_tga(const char * filename)
{
	BitmapImage_t * img = NULL;
	FILE * fp;
	TGAHeader_t hdr;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		goto ERR_EXIT;
	}

	if (load_tga_header(fp, &hdr) < 0) {
		goto ERR_EXIT;
	}

	{
		const char * str = get_image_type_string(hdr.img_type);
		if (str != NULL) {
			printf("Image type: %s\n", str);
		}
		else {
			printf("Image type: Unknown(%d)\n", hdr.img_type);
		}
	}

	printf("Image desc: %02xh\n", hdr.img_desc);
	printf("Color map length: %d\n", hdr.cmap_len);
	printf("Color map size  : %d\n", hdr.cmap_size);
	printf("%d x %d, %dbpp\n"  , hdr.img_width, hdr.img_height, hdr.img_bpp);

	fclose(fp);

ERR_EXIT:
	return img;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
