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

extern int32_t GetBytePerPixel_of_Format(ndz_colorfmt_t fmt);

extern ndz_image_t * convert_RGB888_to_Y8(ndz_image_t * src_img);
extern ndz_image_t * convert_Y8_to_RGB888(ndz_image_t * src_img);

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
