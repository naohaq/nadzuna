/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * bmpfile.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_BMPFILE_H_
#define NADZUNA_BMPFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct T_BMPFILEHEADER {
	uint8_t  bfType[2];
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
};

typedef struct T_BMPFILEHEADER BMPFileHeader_t;

struct T_BMPINFOHEADER {
	uint32_t biSize;
	int32_t  biWidth;
	int32_t  biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t  biXPixPerMeter;
	int32_t  biYPixPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

typedef struct T_BMPINFOHEADER BMPInfoHeader_t;

struct T_BMPFILE {
	BMPFileHeader_t fileHdr;
	BMPInfoHeader_t infoHdr;
	int32_t direction;
	int32_t bpp;
	int32_t width;
	int32_t stride;
	int32_t bytewidth;
	int32_t padded_bytewidth;
	int32_t height;
};

typedef struct T_BMPFILE BMPFile_t;

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_BMPFILE_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
