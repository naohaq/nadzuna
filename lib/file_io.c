/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * file_io.c
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nadzuna.h"

#include "file_io.h"

uint16_t
ndz_read_U16_l(const uint8_t src[2])
{
	uint16_t result = 0;
	result |= (src[0] & 0x00ff);
	result |= (src[1] & 0x00ff) << 8;
	return result;
}

uint16_t
ndz_read_U16_b(const uint8_t src[2])
{
	uint16_t result = 0;
	result |= (src[1] & 0x00ff);
	result |= (src[0] & 0x00ff) << 8;
	return result;
}

int16_t
ndz_read_I16_l(const uint8_t src[2])
{
	int16_t result;
	int32_t val = 0;
	uint32_t s;
	val |= (src[0] & 0x00ff);
	val |= (src[1] & 0x007f) << 8;
	s = (src[1] & 0x0080) >> 7;
	if (s) {
		result = (int16_t)(-32768 + val);
	}
	else {
		result = val;
	}
	return result;
}

int16_t
ndz_read_I16_b(const uint8_t src[2])
{
	int16_t result;
	int32_t val = 0;
	uint32_t s;
	val |= (src[1] & 0x00ff);
	val |= (src[0] & 0x007f) << 8;
	s = (src[0] & 0x0080) >> 7;
	if (s) {
		result = (int16_t)(-32768 + val);
	}
	else {
		result = val;
	}
	return result;
}

uint32_t
ndz_read_U32_l(const uint8_t src[4])
{
	uint32_t result = 0;
	result |=  (src[0] & 0x0ffU);
	result |= ((src[1] & 0x0ffU) <<  8);
	result |= ((src[2] & 0x0ffU) << 16);
	result |= ((src[3] & 0x0ffU) << 24);
	return result;
}

uint32_t
ndz_read_U32_b(const uint8_t src[4])
{
	uint32_t result = 0;
	result |=  (src[3] & 0x0ffU);
	result |= ((src[2] & 0x0ffU) <<  8);
	result |= ((src[1] & 0x0ffU) << 16);
	result |= ((src[0] & 0x0ffU) << 24);
	return result;
}

int32_t
ndz_read_I32_l(const uint8_t src[4])
{
	int32_t result;
	uint32_t val = 0;
	uint32_t s;
	val |=  (src[0] & 0x0ffU);
	val |= ((src[1] & 0x0ffU) <<  8);
	val |= ((src[2] & 0x0ffU) << 16);
	val |= ((src[3] & 0x07fU) << 24);
	s = (src[3] & 0x0080) >> 7;
	if (s) {
		result = 0x80000000 + val;
	}
	else {
		result = val;
	}
	return result;
}

int32_t
ndz_read_I32_b(const uint8_t src[4])
{
	int32_t result;
	uint32_t val = 0;
	uint32_t s;
	val |=  (src[3] & 0x0ffU);
	val |= ((src[2] & 0x0ffU) <<  8);
	val |= ((src[1] & 0x0ffU) << 16);
	val |= ((src[0] & 0x07fU) << 24);
	s = (src[3] & 0x0080) >> 7;
	if (s) {
		result = 0x80000000 + val;
	}
	else {
		result = val;
	}
	return result;
}

void
ndz_write_U16_l(uint8_t * dst, uint16_t val)
{
	uint32_t v;
	v = (val & 0x000ff);
	dst[0] = (uint8_t)v;
	v = (val & 0x0ff00) >> 8;
	dst[1] = (uint8_t)v;
}

void
ndz_write_U16_b(uint8_t * dst, uint16_t val)
{
	uint32_t v;
	v = (val & 0x000ff);
	dst[1] = (uint8_t)v;
	v = (val & 0x0ff00) >> 8;
	dst[0] = (uint8_t)v;
}

void
ndz_write_I16_l(uint8_t * dst, int16_t val)
{
	ndz_write_U16_l(dst, (uint16_t)val);
}

void
ndz_write_I16_b(uint8_t * dst, int16_t val)
{
	ndz_write_U16_b(dst, (uint16_t)val);
}

void
ndz_write_U32_l(uint8_t * dst, uint32_t val)
{
	uint32_t v;
	v = (val & 0x0000000ffU);
	dst[0] = (uint8_t)v;
	v = (val & 0x00000ff00U)>> 8;
	dst[1] = (uint8_t)v;
	v = (val & 0x000ff0000U)>>16;
	dst[2] = (uint8_t)v;
	v = (val & 0x0ff000000U)>>24;
	dst[3] = (uint8_t)v;
}

void
ndz_write_U32_b(uint8_t * dst, uint32_t val)
{
	uint32_t v;
	v = (val & 0x0000000ffU);
	dst[3] = (uint8_t)v;
	v = (val & 0x00000ff00U)>> 8;
	dst[2] = (uint8_t)v;
	v = (val & 0x000ff0000U)>>16;
	dst[1] = (uint8_t)v;
	v = (val & 0x0ff000000U)>>24;
	dst[0] = (uint8_t)v;
}

void
ndz_write_I32_l(uint8_t * dst, int32_t val)
{
	ndz_write_U32_l(dst, (uint32_t)val);
}

void
ndz_write_I32_b(uint8_t * dst, int32_t val)
{
	ndz_write_U32_b(dst, (uint32_t)val);
}

int32_t
ndz_fread_U8str(FILE * fp, uint8_t * ptr, int32_t n)
{
	int32_t ret;
	int32_t ret_val = 1;

	ret = fread(ptr, 1, n, fp);
	if (ret < n) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	return ret_val;
}

int32_t
ndz_fread_U16_l(FILE * fp, uint16_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[2];

	ret = fread(buf, 1, 2, fp);
	if (ret < 2) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_U16_l(buf);
	}
	return ret_val;
}

int32_t
ndz_fread_U16_b(FILE * fp, uint16_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[2];

	ret = fread(buf, 1, 2, fp);
	if (ret < 2) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_U16_b(buf);
	}
	return ret_val;
}

int32_t
ndz_fread_I16_l(FILE * fp, int16_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[2];

	ret = fread(buf, 1, 2, fp);
	if (ret < 1) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_I16_l(buf);
	}
	return ret_val;
}

int32_t
ndz_fread_I16_b(FILE * fp, int16_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[2];

	ret = fread(buf, 1, 2, fp);
	if (ret < 1) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_I16_b(buf);
	}
	return ret_val;
}

int32_t
ndz_fread_U32_l(FILE * fp, uint32_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[4];

	ret = fread(buf, 1, 4, fp);
	if (ret < 4) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_U32_l(buf);
	}

	return ret_val;
}

int32_t
ndz_fread_U32_b(FILE * fp, uint32_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[4];

	ret = fread(buf, 1, 4, fp);
	if (ret < 4) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_U32_b(buf);
	}

	return ret_val;
}

int32_t
ndz_fread_I32_l(FILE * fp, int32_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[4];

	ret = fread(buf, 1, 4, fp);
	if (ret < 4) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_I32_l(buf);
	}

	return ret_val;
}

int32_t
ndz_fread_I32_b(FILE * fp, int32_t * result)
{
	int32_t ret;
	int32_t ret_val = 1;
	uint8_t buf[4];

	ret = fread(buf, 1, 4, fp);
	if (ret < 4) {
		if (feof(fp)) {
			ret_val = 0; /* EOF */
		}
		else {
			ret_val = -1; /* error */
		}
	}
	else {
		*result = ndz_read_I32_b(buf);
	}

	return ret_val;
}

int32_t
ndz_fwrite_U8str(FILE * fp, const uint8_t * ptr, int32_t n)
{
	int32_t ret;
	int32_t ret_val = 0;

	ret = fwrite(ptr, 1, n, fp);
	if (ret < n) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_U16_l(FILE * fp, uint16_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[2];

	ndz_write_U16_l(buf, val);
	ret = fwrite(buf, 1, 2, fp);
	if (ret < 2) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_U16_b(FILE * fp, uint16_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[2];

	ndz_write_U16_b(buf, val);
	ret = fwrite(buf, 1, 2, fp);
	if (ret < 2) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_I16_l(FILE * fp, int16_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[2];

	ndz_write_I16_l(buf, val);
	ret = fwrite(buf, 1, 2, fp);
	if (ret < 2) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_I16_b(FILE * fp, int16_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[2];

	ndz_write_I16_b(buf, val);
	ret = fwrite(buf, 1, 2, fp);
	if (ret < 2) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_U32_l(FILE * fp, uint32_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[4];

	ndz_write_U32_l(buf, val);
	ret = fwrite(buf, 1, 4, fp);
	if (ret < 4) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_U32_b(FILE * fp, uint32_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[4];

	ndz_write_U32_b(buf, val);
	ret = fwrite(buf, 1, 4, fp);
	if (ret < 4) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_I32_l(FILE * fp, int32_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[4];

	ndz_write_I32_l(buf, val);
	ret = fwrite(buf, 1, 4, fp);
	if (ret < 4) {
		ret_val = -1;
	}

	return ret_val;
}

int32_t
ndz_fwrite_I32_b(FILE * fp, int32_t val)
{
	int32_t ret;
	int32_t ret_val = 0;
	uint8_t buf[4];

	ndz_write_I32_b(buf, val);
	ret = fwrite(buf, 1, 4, fp);
	if (ret < 4) {
		ret_val = -1;
	}

	return ret_val;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
