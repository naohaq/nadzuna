/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * bmpfile.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "color.h"
#include "bitmapimg.h"
#include "file_io.h"
#include "bmpfile.h"
#include "error.h"

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


static int
BMP_LoadFileHeader(BMPFileHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int ret_val = 0;

	ret = ndz_fread_U8str(fp, hdr->bfType, 2);
	FREAD_CHECK_ERROR(ret);

	if (hdr->bfType[0] != 'B' || hdr->bfType[1] != 'M') {
		ndz_print_error(__func__, "Incorrect format.");
		ret_val = -1;
	}
	else {
		ret = ndz_fread_U32_l(fp, &(hdr->bfSize));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U16_l(fp, &(hdr->bfReserved1));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U16_l(fp, &(hdr->bfReserved2));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U32_l(fp, &(hdr->bfOffBits));
		FREAD_CHECK_ERROR(ret);
	}

ERR_EXIT:
	return ret_val;
}


static int
BMP_LoadInfoHeader(BMPInfoHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int ret_val = 0;

	ret = ndz_fread_U32_l(fp, &(hdr->biSize));
	FREAD_CHECK_ERROR(ret);

	if (hdr->biSize < 40) {
		ndz_print_error
			(__func__,
			 "Unsupported format: the size of the header is not 40: %d",
			 hdr->biSize);
		ret_val = -1;
	}
	else {
		ret = ndz_fread_I32_l(fp, &(hdr->biWidth));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_I32_l(fp, &(hdr->biHeight));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U16_l(fp, &(hdr->biPlanes));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U16_l(fp, &(hdr->biBitCount));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U32_l(fp, &(hdr->biCompression));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U32_l(fp, &(hdr->biSizeImage));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_I32_l(fp, &(hdr->biXPixPerMeter));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_I32_l(fp, &(hdr->biYPixPerMeter));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U32_l(fp, &(hdr->biClrUsed));
		FREAD_CHECK_ERROR(ret);

		ret = ndz_fread_U32_l(fp, &(hdr->biClrImportant));
		FREAD_CHECK_ERROR(ret);
	}

ERR_EXIT:
	return ret_val;
}

static int
BMP_LoadHeader(BMPFile_t * bmp, FILE * fp)
{
	int ret;
	int ret_val = 0;
	
	ret = BMP_LoadFileHeader(&(bmp->fileHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Can't load BMPFileHeader.");
		ret_val = -1;
		goto ERR_EXIT;
	}

	ret = BMP_LoadInfoHeader(&(bmp->infoHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Can't load BMPInfoHeader.");
		ret_val = -1;
		goto ERR_EXIT;
	}

	/* skip */
	{
		long skiplen = bmp->infoHdr.biSize - 40;
		if (skiplen > 0) {
			ret = fseek(fp, skiplen, SEEK_CUR);
			if (ret < 0) {
				ndz_print_strerror(__func__, "fseek");
				ret_val = -1;
				goto ERR_EXIT;
			}
		}
	}

	if (bmp->infoHdr.biHeight > 0) {
		bmp->direction = -1;
	}
	else {
		bmp->direction = 1;
	}

	bmp->bpp       = bmp->infoHdr.biBitCount;
	bmp->width     = bmp->infoHdr.biWidth;
	bmp->bytewidth = (bmp->width * bmp->bpp)>>3;
	bmp->padded_bytewidth = (bmp->bytewidth + 3) & 0x7ffffffc;
	bmp->height    = (-1) * bmp->infoHdr.biHeight * bmp->direction;

ERR_EXIT:
	return ret_val;
}

static int
BMP_LoadPixels(BMPFile_t * bmp, uint32_t * pixels, FILE * fp)
{
	int err = 0;
	int ret_val = 0;
	int ret;

	uint8_t * line_buf = NULL;

	int32_t width  = bmp->width;
	int32_t height = bmp->height;
	int32_t bpp    = bmp->bpp;
	int32_t padded_bytewidth = bmp->padded_bytewidth;

	if (!(bpp == 24 || bpp == 32) || (bmp->infoHdr.biCompression != 0)) {
		ndz_print_error(__func__, "%dbpp is not supported.", bpp);
		err = 1;
		goto ERR_EXIT;
	}

	line_buf = malloc(padded_bytewidth);
	if (line_buf == NULL) {
		ndz_print_error(__func__, "Memory allocation failed.");
		err = 1;
		goto ERR_EXIT;
	}

	ret = fseek(fp, bmp->fileHdr.bfOffBits, SEEK_SET);
	if (ret < 0) {
		ndz_print_strerror(__func__, "fseek");
		err = 1;
		goto ERR_EXIT;
	}

	for (int i=0; i<height; i++) {
		int yy = (bmp->direction < 0) ? (height - i - 1) : i;
		uint32_t * lp = &(pixels[yy*width]);

		ret = fread(line_buf, 1, padded_bytewidth, fp);
		if (ret < padded_bytewidth) {
			if (feof(fp)) {
				ndz_print_error(__func__, "Unexpected EOF on reading line %d", i+1);
				err = 1;
				goto ERR_EXIT;
			}
			else {
				ndz_print_strerror(__func__, "fread");
				err = 1;
				goto ERR_EXIT;
			}
		}

		if (bpp == 24) {
			for (int j=0; j<width; j++) {
				uint32_t b = line_buf[j*3+0];
				uint32_t g = line_buf[j*3+1];
				uint32_t r = line_buf[j*3+2];
				lp[j] = combine_ARGB(0xff,r,g,b);
			}
		}
		else if (bpp == 32) {
			for (int j=0; j<width; j++) {
				uint32_t b = line_buf[j*4+0];
				uint32_t g = line_buf[j*4+1];
				uint32_t r = line_buf[j*4+2];
				lp[j] = combine_ARGB(0xff,r,g,b);
			}
		}
	}

ERR_EXIT:
	if (line_buf != NULL) {
		free(line_buf);
	}
	if (err) {
		ret_val = -1;
	}

	return ret_val;
}


BitmapImage_t *
load_bmp(const char_t * filename)
{
	int ret;
	int err = 0;
	BitmapImage_t * img = NULL;
	BMPFile_t bmp;
	FILE * fp;

	assert(filename != NULL);

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		err = 1;
		goto ERR_EXIT;
	}

	ret = BMP_LoadHeader(&bmp, fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Failed to load BMP header...");
		err = 1;
		goto ERR_EXIT;
	}

	img = BitmapImage_Create(bmp.width, 0, bmp.height, 32, COLORFMT_ARGB8888_32);
	if (img == NULL) {
		err = 1;
		goto ERR_EXIT;
	}

	ret = BMP_LoadPixels(&bmp, img->pixels, fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Failed to load BMP pixels...");
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


static int
BMP_SetInfoHeader(BMPFile_t * bmp)
{
	bmp->infoHdr.biSize         = 40; /* Fixed number */
	bmp->infoHdr.biWidth        = bmp->width;
	bmp->infoHdr.biHeight       = bmp->height * bmp->direction * (-1);
	bmp->infoHdr.biPlanes       = 1;
	bmp->infoHdr.biBitCount     = bmp->bpp;
	bmp->infoHdr.biCompression  = 0; /* without compression */
	bmp->infoHdr.biSizeImage    = bmp->padded_bytewidth * bmp->height;
	bmp->infoHdr.biXPixPerMeter = 0;
	bmp->infoHdr.biYPixPerMeter = 0;
	bmp->infoHdr.biClrUsed      = 0;
	bmp->infoHdr.biClrImportant = 0;

	return 0;
}


static int
BMP_SetFileHeader(BMPFile_t * bmp)
{
	bmp->fileHdr.bfType[0]   = 'B';
	bmp->fileHdr.bfType[1]   = 'M';
	bmp->fileHdr.bfSize      = 14 + bmp->infoHdr.biSize + bmp->infoHdr.biSizeImage;
	bmp->fileHdr.bfReserved1 = 0;
	bmp->fileHdr.bfReserved2 = 0;
	bmp->fileHdr.bfOffBits   = 14 + bmp->infoHdr.biSize;

	return 0;
}


static int
BMP_StoreFileHeader(BMPFileHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int ret_val = 0;

	ret = ndz_fwrite_U8str(fp, hdr->bfType, 2);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->bfSize);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U16_l(fp, hdr->bfReserved1);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U16_l(fp, hdr->bfReserved2);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->bfOffBits);
	FWRITE_CHECK_ERROR(ret);

ERR_EXIT:
	return ret_val;
}


static int
BMP_StoreInfoHeader(BMPInfoHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int ret_val = 0;

	ret = ndz_fwrite_U32_l(fp, hdr->biSize);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I32_l(fp, hdr->biWidth);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I32_l(fp, hdr->biHeight);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U16_l(fp, hdr->biPlanes);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U16_l(fp, hdr->biBitCount);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->biCompression);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->biSizeImage);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I32_l(fp, hdr->biXPixPerMeter);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_I32_l(fp, hdr->biYPixPerMeter);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->biClrUsed);
	FWRITE_CHECK_ERROR(ret);

	ret = ndz_fwrite_U32_l(fp, hdr->biClrImportant);
	FWRITE_CHECK_ERROR(ret);

ERR_EXIT:
	return ret_val;
}


static int
BMP_StorePixels(BMPFile_t * bmp, const uint32_t * pixels, FILE * fp)
{
	int ret_val = 0;

	uint8_t * line_buf = NULL;
	int32_t stride = bmp->stride;
	int32_t width  = bmp->width;
	int32_t height = bmp->height;
	int32_t bpp    = bmp->bpp;
	int32_t padded_bytewidth = bmp->padded_bytewidth;
	const int32_t y_scale = 1;

	line_buf = malloc(padded_bytewidth);
	if (line_buf == NULL) {
		ndz_print_error(__func__, "Memory allocation failed.");
		ret_val = -1;
		goto ERR_EXIT;
	}

	const uint32_t * lp;
	int32_t line_skip;

	if (bmp->direction < 0) {
		lp = pixels + stride * (height/y_scale - 1);
		line_skip = -stride;
	}
	else {
		lp = pixels;
		line_skip = stride;
	}

	for (int i=0; i<bmp->height; i+=y_scale) {
		if (bpp == 24) {
			for (int j=0; j<width; j++) {
				uint32_t c = lp[j];
				uint32_t b = (c & 0x000000ffU);
				uint32_t g = (c & 0x0000ff00U) >>  8;
				uint32_t r = (c & 0x00ff0000U) >> 16;
				line_buf[j*3+0] = (uint8_t)b;
				line_buf[j*3+1] = (uint8_t)g;
				line_buf[j*3+2] = (uint8_t)r;
			}

		}
		else if (bpp == 32) {
			for (int j=0; j<width; j++) {
				uint32_t c = lp[j];
				uint32_t b = (c & 0x000000ffU);
				uint32_t g = (c & 0x0000ff00U) >>  8;
				uint32_t r = (c & 0x00ff0000U) >> 16;
				line_buf[j*4+0] = (uint8_t)b;
				line_buf[j*4+1] = (uint8_t)g;
				line_buf[j*4+2] = (uint8_t)r;
				line_buf[j*4+3] = 0;
			}

		}

		for (int k=0; k<y_scale; k+=1) {
			size_t ret = fwrite(line_buf, 1, padded_bytewidth, fp);
			if (ret < padded_bytewidth) {
				ndz_print_strerror(__func__, "fwrite");
				ret_val = -1;
				goto ERR_EXIT;
			}
		}

		lp += line_skip;
	}		

ERR_EXIT:
	if (line_buf != NULL) {
		free(line_buf);
	}
	return ret_val;
}


int
save_bmp24(const char_t * filename, const uint32_t * pixels, int32_t w, int32_t stride, int32_t h)
{
	int ret;
	int ret_val = 0;
	BMPFile_t bmp;
	FILE * fp = NULL;

	assert(filename != NULL);
	assert(pixels != NULL);

	if (stride == 0) {
		stride = w;
	}
	else if (stride < w) {
		ndz_print_error(__func__, "The stride must be greater than or equal to the width: %d < %d", stride, w);
		ret_val = -1;
		goto ERR_EXIT;
	}

	bmp.width  = w;
	bmp.stride = stride;
	bmp.height = h;
	bmp.bpp    = 24;
	bmp.direction = -1;

	bmp.bytewidth = (bmp.width * bmp.bpp)>>3;
	bmp.padded_bytewidth = (bmp.bytewidth + 3) & 0x7ffffffc;

	BMP_SetInfoHeader(&bmp);
	BMP_SetFileHeader(&bmp);

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		ret_val = -1;
		goto ERR_EXIT;
	}

	ret = BMP_StoreFileHeader(&(bmp.fileHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Failed to write file header.");
		ret_val = -1;
		goto ERR_EXIT;
	}

	ret = BMP_StoreInfoHeader(&(bmp.infoHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Failed to write info header.");
		ret_val = -1;
		goto ERR_EXIT;
	}

	ret = BMP_StorePixels(&bmp, pixels, fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Failed to write pixels.");
		ret_val = -1;
	}

ERR_EXIT:
	if (fp != NULL) {
		fclose(fp);
	}
	return ret_val;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
