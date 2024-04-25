/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * nadzuna.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_H_
#define NADZUNA_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>

/* Some compilers use a special export keyword */
#ifndef NADZUNA_API
# if defined(__WIN32__) || defined(__WINRT__) || defined(__CYGWIN__) || defined(__GDK__)
#  ifdef NADZUNA_BUILD
#   define NADZUNA_API __declspec(dllexport)
#  else
#   define NADZUNA_API
#  endif
# elif defined(__OS2__)
#   ifdef NADZUNA_BUILD
#    define NADZUNA_API  __declspec(dllexport)
#   else
#    define NADZUNA_API
#   endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define NADZUNA_API __attribute__ ((visibility("default")))
#  else
#   define NADZUNA_API
#  endif
# endif
#endif /* NADZUNA_API */


enum T_NDZ_COLOR_FORMAT {
	NDZ_COLORFMT_NONE,

	NDZ_COLORFMT_ARGB8888,

	NDZ_COLORFMT_RGB888,

	NDZ_COLORFMT_RGB565,
	NDZ_COLORFMT_ARGB1555,

	NDZ_COLORFMT_Y8,

	/* aliases */
	NDZ_COLORFMT_GRAYSCALE   = NDZ_COLORFMT_Y8,
};

typedef enum T_NDZ_COLOR_FORMAT ndz_colorfmt_t;

struct T_NDZ_YUV_COLOR {
	uint8_t y;
	uint8_t u;
	uint8_t v;
};

typedef struct T_NDZ_YUV_COLOR ndz_yuv_t;

struct T_NADZUNA_IMAGE {
	int32_t width;
	int32_t height;
	int32_t stride;
	int32_t bpp;
	ndz_colorfmt_t fmt;
	void * pixels;
};

typedef struct T_NADZUNA_IMAGE ndz_image_t;

extern NADZUNA_API ndz_image_t * ndz_image_create(int32_t width, int32_t stride, int32_t height, int32_t bpp, ndz_colorfmt_t fmt);
extern NADZUNA_API void ndz_image_free(ndz_image_t * img);

extern NADZUNA_API ndz_image_t * ndz_load_png(const char * filename);

extern NADZUNA_API ndz_image_t * ndz_load_jpeg(const char * filename);

extern NADZUNA_API ndz_image_t * ndz_load_tga(const char * filename);
extern NADZUNA_API int ndz_save_tga(const char * filename, ndz_image_t * img);

extern NADZUNA_API ndz_image_t * ndz_load_bmp(const char * filename);
extern NADZUNA_API int ndz_save_bmp(const char * filename, ndz_image_t * img);

extern NADZUNA_API ndz_image_t * ndz_load_pgm(const char * filename);
extern NADZUNA_API int ndz_save_pgm(const char * filename, ndz_image_t * img);

extern NADZUNA_API int ndz_bytes_per_pixel(ndz_colorfmt_t fmt);

extern NADZUNA_API ndz_yuv_t ndz_rgb2yuv(uint32_t rgb);
extern NADZUNA_API uint32_t ndz_yuv2rgb(ndz_yuv_t s);


#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
