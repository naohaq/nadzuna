/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * error.c
 * 
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "nadzuna.h"

#include "common.h"
#include "error.h"

int32_t
ndz_print_strerror(const char_t * func_name, const char_t * err_func)
{
	int32_t ret;
	char_t buf[256];
#ifdef _WIN32
	const char_t * estr;
	estr = strerror(errno);
	strncpy(buf, estr, 256);
	buf[255] = '\0';
#else
	buf[0] = '\0';
	strerror_r(errno, buf, 256);
	buf[255] = '\0';
#endif
	ret = fprintf(stderr, "%s: %s: %s\n", func_name, err_func, buf);
	return ret;
}

int32_t
ndz_print_error(const char_t * func_name, const char_t * fmt, ...)
{
	int32_t ret;
	int32_t rc;
	va_list ap;

	va_start(ap, fmt);

	ret = fprintf(stderr, "%s: ", func_name);
	ret = vfprintf(stderr, fmt, ap);
	rc = ret;

	va_end(ap);

	ret = fputs("\n", stderr);
	return rc;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
