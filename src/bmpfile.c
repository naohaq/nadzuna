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

static int32_t
BMP_LoadFileHeader(BMPFileHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int32_t ret_val = 0;

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


static int32_t
BMP_LoadInfoHeader(BMPInfoHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int32_t ret_val = 0;

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

static int32_t
BMP_LoadHeader(BMPFile_t * bmp, FILE * fp)
{
	int32_t ret;
	
	ret = BMP_LoadFileHeader(&(bmp->fileHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Can't load BMPFileHeader.");
		return -1;
	}

	ret = BMP_LoadInfoHeader(&(bmp->infoHdr), fp);
	if (ret < 0) {
		ndz_print_error(__func__, "Can't load BMPInfoHeader.");
		return -1;
	}

	/* skip */
	{
		int32_t skiplen = bmp->infoHdr.biSize - 40;
		if (skiplen > 0) {
			ret = fseek(fp, skiplen, SEEK_CUR);
			if (ret < 0) {
				ndz_print_strerror(__func__, "fseek");
				return -1;
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

	bmp->pixels = NULL;
	
	return 0;
}

static int32_t
BMP_LoadPixels(BMPFile_t * bmp, FILE * fp)
{
	int32_t ret_val = 0;
	int32_t ret;
	uint8_t * line_buf;
	int32_t pixel_count;
	int32_t width = bmp->width;
	int32_t bpp = bmp->bpp;
	int32_t padded_bytewidth = bmp->padded_bytewidth;

	if (!(bpp == 24 || bpp == 32) || (bmp->infoHdr.biCompression != 0)) {
		ndz_print_error(__func__, "%dbpp is not supported.", bpp);
		return -1;
	}

	line_buf = malloc(padded_bytewidth);
	if (line_buf == NULL) {
		ndz_print_error(__func__, "Memory allocation failed.");
		return -1;
	}

	pixel_count = width * bmp->height;
	bmp->pixels = malloc(sizeof(uint32_t)*pixel_count);
	if (bmp->pixels == NULL) {
		ndz_print_error(__func__, "Memory allocation failed.");
		free(line_buf);
		return -1;
	}

	ret = fseek(fp, bmp->fileHdr.bfOffBits, SEEK_SET);
	if (ret < 0) {
		ndz_print_strerror(__func__, "fseek");
		return -1;
	}

	{
		int32_t i;
		uint32_t * lp;
		int32_t line_skip;

		if (bmp->direction < 0) {
			lp = bmp->pixels + width * (bmp->height - 1);
			line_skip = -width;
		}
		else {
			lp = bmp->pixels;
			line_skip = width;
		}

		for (i=0; i<bmp->height; i++) {
			ret = fread(line_buf, 1, padded_bytewidth, fp);
			if (ret < padded_bytewidth) {
				if (feof(fp)) {
					ndz_print_error(__func__, "Unexpected EOF when reading line %d: %d", i+1, ret);
					free(bmp->pixels);
					bmp->pixels = NULL;
					ret_val = -1;
					break;
				}
				else {
					ndz_print_strerror(__func__, "fread");
					free(bmp->pixels);
					bmp->pixels = NULL;
					ret_val = -1;
					break;
				}
			}

			if (bpp == 24) {
				int32_t j;
				for (j=0; j<width; j++) {
					uint32_t c = 0x00000000U;
					uint32_t b = line_buf[j*3+0];
					uint32_t g = line_buf[j*3+1];
					uint32_t r = line_buf[j*3+2];
					c  = (r&0x0ffU)<<16;
					c |= (g&0x0ffU)<< 8;
					c |= (b&0x0ffU);
					lp[j] = c;
				}
			}
			else if (bpp == 32) {
				int32_t j;
				for (j=0; j<width; j++) {
					uint32_t c = 0x00000000U;
					uint32_t b = line_buf[j*4+0];
					uint32_t g = line_buf[j*4+1];
					uint32_t r = line_buf[j*4+2];
					c  = (r&0x0ffU)<<16;
					c |= (g&0x0ffU)<< 8;
					c |= (b&0x0ffU);
					lp[j] = c;
				}
			}

			lp += line_skip;
		}

		free(line_buf);
	}

	return ret_val;
}


int32_t
load_bmp(const char_t * filename, BitmapImage_t * img)
{
	int32_t ret;
	int32_t ret_val = 0;
	BMPFile_t bmp;
	FILE * fp;

	assert(filename != NULL);
	assert(img != NULL);

	img->pixels = NULL;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		ret_val = -1;
	}
	else {
		ret = BMP_LoadHeader(&bmp, fp);
		if (ret < 0) {
			ndz_print_error(__func__, "Failed to load BMP header...");
			ret_val = -1;
		}
		else {
			ret = BMP_LoadPixels(&bmp, fp);
			if (ret < 0) {
				ndz_print_error(__func__, "Failed to load BMP pixels...");
				ret_val = -1;
			}
		}

		fclose(fp);

		if (! (ret_val < 0)) {
			img->pixels = bmp.pixels;
			img->width  = bmp.width;
			img->height = bmp.height;
			img->stride = 0;
			img->bpp    = 24;
			img->align  = 2;
		}
	}

	return ret_val;
}

static int32_t
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

static int32_t
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

static int32_t
BMP_StoreFileHeader(BMPFileHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int32_t ret_val = 0;

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

static int32_t
BMP_StoreInfoHeader(BMPInfoHeader_t * hdr, FILE * fp)
{
	int32_t ret;
	int32_t ret_val = 0;

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

static int32_t
BMP_StorePixels(BMPFile_t * bmp, const uint32_t * pixels, FILE * fp)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t * line_buf;
	int32_t stride = bmp->stride;
	int32_t width = bmp->width;
	int32_t bpp = bmp->bpp;
	int32_t padded_bytewidth = bmp->padded_bytewidth;
	const int32_t y_scale = 1;

	line_buf = malloc(padded_bytewidth);
	if (line_buf == NULL) {
		ndz_print_error(__func__, "Memory allocation error.");
		ret_val = -1;
	}
	else {
		int32_t i;
		int32_t k;
		const uint32_t * lp;
		int32_t line_skip;

		if (bmp->direction < 0) {
			lp = pixels + stride * (bmp->height/y_scale - 1);
			line_skip = -stride;
		}
		else {
			lp = pixels;
			line_skip = stride;
		}

		for (i=0; i<bmp->height; i+=y_scale) {
			if (bpp == 24) {
				int32_t j;
				for (j=0; j<width; j++) {
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
				int32_t j;
				for (j=0; j<width; j++) {
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

			for (k=0; k<y_scale; k+=1) {
				ret = fwrite(line_buf, 1, padded_bytewidth, fp);
				if (ret < padded_bytewidth) {
					ndz_print_strerror(__func__, "fwrite");
					ret_val = -1;
					break;
				}
			}

			if (ret_val < 0) {
				break;
			}

			lp += line_skip;
		}		

		free(line_buf);
	}

	return ret_val;
}

int32_t
save_bmp24(const char_t * filename, const uint32_t * pixels, int32_t w, int32_t stride, int32_t h)
{
	int32_t ret;
	int32_t ret_val = 0;
	BMPFile_t bmp;
	FILE * fp;

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

	bmp.pixels = NULL;

	BMP_SetInfoHeader(&bmp);
	BMP_SetFileHeader(&bmp);

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		ndz_print_strerror(__func__, "fopen");
		ret_val = -1;
	}
	else {
		ret = BMP_StoreFileHeader(&(bmp.fileHdr), fp);
		if (ret < 0) {
			ndz_print_error(__func__, "Failed to write file header.");
			ret_val = -1;
		}
		else {
			ret = BMP_StoreInfoHeader(&(bmp.infoHdr), fp);
			if (ret < 0) {
				ndz_print_error(__func__, "Failed to write info header.");
				ret_val = -1;
			}
			else {
				ret = BMP_StorePixels(&bmp, pixels, fp);
				if (ret < 0) {
					ndz_print_error(__func__, "Failed to write pixels.");
					ret_val = 1;
				}
			}
		}

		fclose(fp);
	}

 ERR_EXIT:
	return ret_val;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
