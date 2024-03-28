/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * bitmapimg.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_BITMAPIMG_H_
#define NADZUNA_BITMAPIMG_H_

#ifdef __cplusplus
extern "C" {
#endif

struct T_BITMAP_IMAGE {
	int32_t width;
	int32_t height;
	int32_t stride;
	int32_t bpp;
	ColorFormat_t fmt;
	int32_t align;
	void * pixels;
};

typedef struct T_BITMAP_IMAGE BitmapImage_t;

extern BitmapImage_t * BitmapImage_Create(int32_t width, int32_t stride, int32_t height, int32_t bpp, ColorFormat_t fmt);
extern void BitmapImage_Free(BitmapImage_t * img);

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_BITMAPIMG_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
