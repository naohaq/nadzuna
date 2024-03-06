/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * pgmfile.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_PGMFILE_H_
#define NADZUNA_PGMFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

BitmapImage_t * load_pgm(const char * filename);
int32_t save_pgm(const char * filename, BitmapImage_t * img);

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_PGMFILE_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
