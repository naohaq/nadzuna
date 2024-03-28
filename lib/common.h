/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * common.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_COMMON_H_
#define NADZUNA_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IMG_MAX_WIDTH    (16384)
#define IMG_MAX_HEIGHT   (16384)

#define IMG_CHECK_DIMENSIONS(w,h)  \
    (((w)>0)&&((w)<=IMG_MAX_WIDTH)&&((h)>0)&&((h)<=IMG_MAX_HEIGHT))

typedef char char_t;
typedef float float32_t;

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_COMMON_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
