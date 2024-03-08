/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * color.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_COLOR_H_
#define NADZUNA_COLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

enum T_COLOR_FORMAT {
	COLORFMT_ARGB8888_32 = 0,
#define COLORFMT_RGB888_32 COLORFMT_ARGB8888_32
	COLORFMT_BGR888_32   = 1,
	COLORFMT_RGB565_16   = 2,
	COLORFMT_ARGB1555_16 = 3,
	COLORFMT_YUV444_32   = 4,
	COLORFMT_YUV422_16   = 5,
	COLORFMT_Y8          = 6,
};

typedef enum T_COLOR_FORMAT ColorFormat_t;

struct T_YUV444_COLOR {
	uint8_t y;
	uint8_t u;
	uint8_t v;
};

typedef struct T_YUV444_COLOR yuv444_color_t;
typedef struct T_BITMAP_IMAGE BitmapImage_t;

static inline uint32_t
combine_ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t c = 0;
	c |= ((uint32_t)a & 0x000000ff) << 24;
	c |= ((uint32_t)r & 0x000000ff) << 16;
	c |= ((uint32_t)g & 0x000000ff) <<  8;
	c |= ((uint32_t)b & 0x000000ff);
	return c;
}

extern int32_t GetBytePerPixel_of_Format(ColorFormat_t fmt);
extern yuv444_color_t RGB_to_YUV(uint32_t rgb);
extern uint32_t YUV_to_RGB(yuv444_color_t s);

extern BitmapImage_t * convert_RGB888_to_Y8(BitmapImage_t * src_img);
extern BitmapImage_t * convert_Y8_to_RGB888(BitmapImage_t * src_img);

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_COLOR_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
