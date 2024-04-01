/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * tgafile.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_TGAFILE_H_
#define NADZUNA_TGAFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_TGAFILE_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
