/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * color_inline.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_COLOR_INLINE_H_
#define NADZUNA_COLOR_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_COLOR_INLINE_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
