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
#include <string.h>
#include <errno.h>

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "file_io.h"
#include "tgafile.h"
#include "error.h"

/* TGA pixel formats */
#define TGA_TYPE_MAPPED       (1)
#define TGA_TYPE_COLOR        (2)
#define TGA_TYPE_GRAY         (3)
#define TGA_TYPE_MAPPED_RLE   (9)
#define TGA_TYPE_COLOR_RLE   (10)
#define TGA_TYPE_GRAY_RLE    (11)

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

static int TGA_load_header(FILE * fp, TGAHeader_t * hdr);

#define FREAD_CHECK_ERROR(x) do {                        \
        if ((x) == 0) {                                  \
            err = 1;                                     \
            ndz_print_error(__func__, "Unexpected EOF"); \
            goto ERR_EXIT;                               \
        }                                                \
        else if ((x) == -1) {                            \
            err = 1;                                     \
            ndz_print_strerror(__func__, "fread");       \
            goto ERR_EXIT;                               \
        }                                                \
    } while(0)

#define FWRITE_CHECK_ERROR(x) do {                  \
        if ((x) < 0) {                              \
            err = 1;                                \
            ndz_print_strerror(__func__, "fwrite"); \
            goto ERR_EXIT;                          \
        }                                           \
    } while (0)                                     \


static int
TGA_load_header(FILE * fp, TGAHeader_t * phdr)
{
	int32_t ret;
	int ret_val = 0;
	int err = 0;

	assert(fp != NULL);
	assert(phdr != NULL);

	ret = ndz_fread_U8str(fp, &(phdr->id_len), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(phdr->cmap_type), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(phdr->img_type), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->cmap_org));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->cmap_len));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(phdr->cmap_size), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->img_org_x));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->img_org_y));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->img_width));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_I16_l(fp, &(phdr->img_height));
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(phdr->img_bpp), 1);
	FREAD_CHECK_ERROR(ret);

	ret = ndz_fread_U8str(fp, &(phdr->img_desc), 1);
	FREAD_CHECK_ERROR(ret);

ERR_EXIT:
	if (err) {
		ret_val = -1;
	}
	return ret_val;
}


static int
TGA_store_header(FILE * fp, TGAHeader_t * phdr)
{
	int32_t ret;
	int ret_val = 0;
	int err = 0;

	assert(fp != NULL);
	assert(phdr != NULL);

	ret = ndz_fwrite_U8str(fp, &(phdr->id_len), 1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U8str(fp, &(phdr->cmap_type), 1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U8str(fp, &(phdr->img_type), 1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->cmap_org);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->cmap_len);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U8str(fp, &(phdr->cmap_size), 1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->img_org_x);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->img_org_y);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->img_width);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I16_l(fp, phdr->img_height);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U8str(fp, &(phdr->img_bpp), 1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U8str(fp, &(phdr->img_desc), 1);
	FWRITE_CHECK_ERROR(ret);

ERR_EXIT:
	if (err) {
		ret_val = -1;
	}
	return ret_val;
}

static int
TGA_load_colormap(FILE * fp, const TGAHeader_t * phdr, uint32_t * coltbl)
{
	int err = 0;
	int has_alpha = 0;
	int cnt = 0;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(coltbl != NULL);

	int n = phdr->cmap_len;
	int s = phdr->cmap_size;

	if (s == 16 && ((phdr->img_desc & 0x0f) != 0)) {
		has_alpha = 1;
	}

	switch (s) {
	case 16: {
		uint8_t a_mask = has_alpha ? 0x00 : 0xff;
		for (int k=0; k<n; k+=1) {
			uint16_t c;
			int32_t ret = ndz_fread_U16_l(fp, &c);
			FREAD_CHECK_ERROR(ret);

			uint8_t a = (c&0x8000)>>15;
			uint8_t r = (c&0x7c00)>>10;
			uint8_t g = (c&0x03e0)>> 5;
			uint8_t b = (c&0x001f);

			coltbl[k] = combine_ARGB((0xff*a)|a_mask,r<<3,g<<3,b<<3);
			cnt += 1;
		}
		break;
	}

	case 24: {
		for (int k=0; k<n; k+=1) {
			uint8_t rgb[3];
			int32_t ret = ndz_fread_U8str(fp, &(rgb[0]), 3);
			FREAD_CHECK_ERROR(ret);

			coltbl[k] = combine_ARGB(0xff,rgb[0],rgb[1],rgb[2]);
			cnt += 1;
		}
		break;
	}

	case 32: {
		for (int k=0; k<n; k+=1) {
			uint32_t c;
			int32_t ret = ndz_fread_U32_l(fp, &c);
			FREAD_CHECK_ERROR(ret);

			coltbl[k] = c;
			cnt += 1;
		}
		break;
	}

	default:
		err = 1;
		goto ERR_EXIT;
	}

ERR_EXIT:
	if (err) {
		cnt = -1;
	}
	return cnt;
}


static BitmapImage_t *
TGA_load_colormapped_image(FILE * fp, const TGAHeader_t * phdr, int w, int h, int bpp, int vflip)
{
	int err = 0;
	uint32_t * coltbl = NULL;
	uint8_t * pxbuf = NULL;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(bpp == 8);

	BitmapImage_t * img = BitmapImage_Create(w, 0, h, 32, COLORFMT_ARGB8888_32);
	if (img == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	coltbl = malloc(sizeof(uint32_t) * phdr->cmap_len);
	if (coltbl == NULL) {
		err = 1;
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	if (TGA_load_colormap(fp, phdr, coltbl) < 0) {
		err = 1;
		goto ERR_EXIT;
	}

	pxbuf = malloc(w);
	if (pxbuf == NULL) {
		err = 1;
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	uint32_t * pixels = (uint32_t *)img->pixels;
	for (int y=0; y<h; y+=1) {
		int yy = vflip ? (h - y - 1) : y;
		uint32_t * dlp = &(pixels[yy * img->stride]);
		int32_t ret = ndz_fread_U8str(fp, pxbuf, w);
		FREAD_CHECK_ERROR(ret);

		for (int k=0; k<w; k+=1) {
			uint8_t idx = pxbuf[k];
			uint32_t c = coltbl[idx];
			dlp[k] = c;
		}
	}

ERR_EXIT:
	if (pxbuf != NULL) {
		free(pxbuf);
	}

	if (coltbl != NULL) {
		free(coltbl);
	}

	if (err) {
		if (img != NULL) {
			BitmapImage_Free(img);
			img = NULL;
		}
	}
	return img;
}


static BitmapImage_t *
TGA_load_fullcolor_image(FILE * fp, const TGAHeader_t * phdr, int w, int h, int bpp, int vflip)
{
	int err = 0;
	int has_alpha = 0;
	uint8_t * pxbuf = NULL;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(bpp == 16 || bpp == 24 || bpp == 32);

	BitmapImage_t * img = BitmapImage_Create(w, 0, h, 32, COLORFMT_ARGB8888_32);
	if (img == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	int bytepp = (bpp + 7) >> 3;
	pxbuf = malloc(w*bytepp);
	if (pxbuf == NULL) {
		err = 1;
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	if (bpp == 16 && ((phdr->img_desc & 0x0f) != 0)) {
		has_alpha = 1;
	}

	uint8_t a_mask = has_alpha ? 0x00 : 0xff;
	uint32_t * pixels = (uint32_t *)img->pixels;
	for (int y=0; y<h; y+=1) {
		int yy = vflip ? (h - y - 1) : y;
		uint32_t * dlp = &(pixels[yy * img->stride]);
		int32_t ret = ndz_fread_U8str(fp, pxbuf, w*bytepp);
		FREAD_CHECK_ERROR(ret);

		switch (bpp) {
		case 16:
			for (int k=0; k<w; k+=1) {
				uint16_t c = ndz_read_U16_l(&(pxbuf[k*2]));
				uint8_t a = (c&0x8000)>>15;
				uint8_t r = (c&0x7c00)>>10;
				uint8_t g = (c&0x03e0)>> 5;
				uint8_t b = (c&0x001f);
				dlp[k] = combine_ARGB((0xff*a)|a_mask,r<<3,g<<3,b<<3);
			}
			break;

		case 24:
			for (int k=0; k<w; k+=1) {
				uint8_t b = pxbuf[k*3+0];
				uint8_t g = pxbuf[k*3+1];
				uint8_t r = pxbuf[k*3+2];
				dlp[k] = combine_ARGB(0xff,r,g,b);
			}
			break;

		case 32:
			for (int k=0; k<w; k+=1) {
				uint8_t b = pxbuf[k*4+0];
				uint8_t g = pxbuf[k*4+1];
				uint8_t r = pxbuf[k*4+2];
				uint8_t a = pxbuf[k*4+3];
				dlp[k] = combine_ARGB(a,r,g,b);
			}
			break;

		default:
			/* must not be reached. */
			ndz_print_error(__func__, "[BUG] illegal bpp value: %d", bpp);
			abort( );
		}
	}

ERR_EXIT:
	if (pxbuf != NULL) {
		free(pxbuf);
	}

	if (err) {
		if (img != NULL) {
			BitmapImage_Free(img);
			img = NULL;
		}
	}
	return img;
}


static BitmapImage_t *
TGA_load_grayscale_image(FILE * fp, const TGAHeader_t * phdr, int w, int h, int bpp, int vflip)
{
	int err = 0;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(bpp == 8);

	BitmapImage_t * img = BitmapImage_Create(w, 0, h, 8, COLORFMT_Y8);
	if (img == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	int bytepp = (bpp + 7) >> 3;
	int stride = img->stride;
	uint8_t * pixels = (uint8_t *)img->pixels;
	for (int y=0; y<h; y+=1) {
		int yy = vflip ? (h - y - 1) : y;
		uint8_t * dlp = &(pixels[yy * stride]);
		int32_t ret = ndz_fread_U8str(fp, dlp, w*bytepp);
		FREAD_CHECK_ERROR(ret);
	}

ERR_EXIT:
	if (err) {
		if (img != NULL) {
			BitmapImage_Free(img);
			img = NULL;
		}
	}
	return img;
}

static int
TGA_store_fullcolor_image(FILE * fp, const TGAHeader_t * phdr, const BitmapImage_t * img)
{
	int err = 0;
	int ret_val = 0;
	uint8_t * pxbuf = NULL;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(img != NULL);

	int w = img->width;
	int h = img->height;
	int bpp = phdr->img_bpp;
	int bytepp = (bpp + 7) >> 3;

	pxbuf = malloc(w*bytepp);
	if (pxbuf == NULL) {
		err = 1;
		ndz_print_error(__func__, "Failed to allocate memory...");
		goto ERR_EXIT;
	}

	for (int y=0; y<h; y+=1) {
		int yy = h - y - 1; /* flip vertically */
		int32_t ret;

		switch (img->fmt) {
		case COLORFMT_ARGB8888_32: {
			uint32_t * slp = &(((uint32_t *)img->pixels)[yy * img->stride]);
			for (int k=0; k<w; k+=1) {
				ndz_write_U32_l(&(pxbuf[k*4]), slp[k]);
			}
			break;
		}

		case COLORFMT_ARGB1555_16: {
			uint16_t * slp = &(((uint16_t *)img->pixels)[yy * img->stride]);
			for (int k=0; k<w; k+=1) {
				ndz_write_U16_l(&(pxbuf[k*2]), slp[k]);
			}
			break;
		}

		default:
			/* must not be reached. */
			ndz_print_error(__func__, "[BUG] illegal image format: %d", img->fmt);
			abort( );
		}

		ret = ndz_fwrite_U8str(fp, pxbuf, w*bytepp);
		FWRITE_CHECK_ERROR(ret);
	}

ERR_EXIT:
	if (pxbuf != NULL) {
		free(pxbuf);
	}

	if (err) {
		ret_val = -1;
	}
	return ret_val;
}


static int
TGA_store_grayscale_image(FILE * fp, const TGAHeader_t * phdr, const BitmapImage_t * img)
{
	int err = 0;
	int ret_val = 0;

	assert(fp != NULL);
	assert(phdr != NULL);
	assert(img != NULL);

	int w = img->width;
	int h = img->height;
	int stride = img->stride;

	uint8_t * pixels = (uint8_t *)img->pixels;

	for (int y=0; y<h; y+=1) {
		int yy = h - y - 1; /* flip vertically */
		const uint8_t * slp = &(pixels[yy * stride]);
		int ret = ndz_fwrite_U8str(fp, slp, w);
		FWRITE_CHECK_ERROR(ret);
	}

ERR_EXIT:
	if (err) {
		ret_val = -1;
	}
	return ret_val;
}

BitmapImage_t *
load_tga(const char * filename)
{
	BitmapImage_t * img = NULL;
	int err = 0;
	FILE * fp;

	assert(filename != NULL);

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		err = 1;
		goto ERR_EXIT;
	}

	TGAHeader_t hdr;
	if (TGA_load_header(fp, &hdr) < 0) {
		ndz_print_error(__func__, "Failed to load TGA header");
		err = 1;
		goto ERR_EXIT;
	}

	int w = hdr.img_width;
	int h = hdr.img_height;
	if (! IMG_CHECK_DIMENSIONS(w, h)) {
		ndz_print_error(__func__, "Image size is out of range: %d x %d", w, h);
		err = 1;
		goto ERR_EXIT;
	}

	int vflip = 1;
	if ((hdr.img_desc & 0x20) != 0) {
		vflip = 0;
	}

	int bpp = hdr.img_bpp;
	switch (hdr.img_type) {
	case TGA_TYPE_MAPPED:
		if (bpp != 8) {
			ndz_print_error(__func__, "Color-mapped %d bpp image is not supported", bpp);
			err = 1;
			goto ERR_EXIT;
		}

		img = TGA_load_colormapped_image(fp, &hdr, w, h, bpp, vflip);
		break;

	case TGA_TYPE_COLOR:
		if (bpp != 16 && bpp != 24 && bpp != 32) {
			ndz_print_error(__func__, "Full color %d bpp image is not supported", bpp);
			err = 1;
			goto ERR_EXIT;
		}

		img = TGA_load_fullcolor_image(fp, &hdr, w, h, bpp, vflip);
		break;

	case TGA_TYPE_GRAY:
		if (bpp != 8) {
			ndz_print_error(__func__, "Grayscale %d bpp image is not supported", bpp);
			err = 1;
			goto ERR_EXIT;
		}

		img = TGA_load_grayscale_image(fp, &hdr, w, h, bpp, vflip);
		break;

	case TGA_TYPE_MAPPED_RLE:
	case TGA_TYPE_COLOR_RLE:
	case TGA_TYPE_GRAY_RLE:
		ndz_print_error(__func__, "RLE is not supported");
		err = 1;
		goto ERR_EXIT;
		break;

	default:
		ndz_print_error(__func__, "Unknown image type: %d", hdr.img_type);
		err = 1;
		goto ERR_EXIT;
	}


ERR_EXIT:
	if (fp != NULL) {
		fclose(fp);
	}
	if (err) {
		if (img != NULL) {
			BitmapImage_Free(img);
			img = NULL;
		}
	}
	return img;
}

int
save_tga(const char * filename, BitmapImage_t * img)
{
	int err = 0;
	int ret_val = 0;
	FILE * fp = NULL;

	assert(filename != NULL);
	assert(img != NULL);

	TGAHeader_t hdr;

	memset(&hdr, 0, sizeof(TGAHeader_t));

	hdr.id_len    = 0;
	hdr.cmap_type = 0;
	hdr.cmap_org  = 0;
	hdr.cmap_len  = 0;
	hdr.cmap_size = 0;
	hdr.img_org_x = 0;
	hdr.img_org_y = 0;

	hdr.img_width  = img->width;
	hdr.img_height = img->height;

	switch (img->fmt) {
	case COLORFMT_ARGB8888_32:
		hdr.img_type = TGA_TYPE_COLOR;
		hdr.img_bpp  = 32;
		hdr.img_desc = 0x08;
		break;

	case COLORFMT_ARGB1555_16:
		hdr.img_type = TGA_TYPE_COLOR;
		hdr.img_bpp  = 16;
		hdr.img_desc = 0x01;
		break;

	case COLORFMT_Y8:
		hdr.img_type = TGA_TYPE_GRAY;
		hdr.img_bpp  = 8;
		hdr.img_desc = 0x00;
		break;

	default:
		ndz_print_error(__func__, "Unsupported color format: %d\n", img->fmt);
		err = 1;
		goto ERR_EXIT;
	}

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		err = 1;
		goto ERR_EXIT;
	}

	if (TGA_store_header(fp, &hdr) < 0) {
		ndz_print_error(__func__, "Failed to store TGA header");
		err = 1;
		goto ERR_EXIT;
	}

	switch (img->fmt) {
	case COLORFMT_ARGB8888_32:
	case COLORFMT_ARGB1555_16:
		if (TGA_store_fullcolor_image(fp, &hdr, img) < 0) {
			err = 1;
			goto ERR_EXIT;
		}
		break;

	case COLORFMT_Y8:
		if (TGA_store_grayscale_image(fp, &hdr, img) < 0) {
			err = 1;
			goto ERR_EXIT;
		}
		break;

	default:
		/* must not be reached. */
		ndz_print_error(__func__, "[BUG] illegal image format: %d", img->fmt);
		abort( );
	}

ERR_EXIT:
	if (fp != NULL) {
		fclose(fp);
	}
	if (err) {
		ret_val = -1;
	}
	return ret_val;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
