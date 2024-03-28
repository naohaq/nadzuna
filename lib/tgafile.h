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

extern BitmapImage_t * load_tga(const char * filename);
extern int save_tga(const char * filename, BitmapImage_t * img);

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
