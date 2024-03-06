/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * error.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_ERROR_H_
#define NADZUNA_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t ndz_print_strerror(const char_t * func_name, const char_t * err_func);
extern int32_t ndz_print_error(const char_t * func_name, const char_t * fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_ERROR_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
