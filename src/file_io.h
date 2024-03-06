/* -*- mode: c; coding: utf-8-unix -*- */
/*
 * file_io.h
 *
 *  by Naoyuki MORITA <naohaq@ps.sakura.ne.jp>
 */

#ifndef NADZUNA_FILE_IO_H_
#define NADZUNA_FILE_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t ndz_read_U16_l(const uint8_t src[2]);
extern uint16_t ndz_read_U16_b(const uint8_t src[2]);
extern int16_t  ndz_read_I16_l(const uint8_t src[2]);
extern int16_t  ndz_read_I16_b(const uint8_t src[2]);
extern uint32_t ndz_read_U32_l(const uint8_t src[4]);
extern uint32_t ndz_read_U32_b(const uint8_t src[4]);
extern int32_t  ndz_read_I32_l(const uint8_t src[4]);
extern int32_t  ndz_read_I32_b(const uint8_t src[4]);

extern void     ndz_write_U16_l(uint8_t * dst, uint16_t val);
extern void     ndz_write_U16_b(uint8_t * dst, uint16_t val);
extern void     ndz_write_I16_l(uint8_t * dst, int16_t  val);
extern void     ndz_write_I16_b(uint8_t * dst, int16_t  val);
extern void     ndz_write_U32_l(uint8_t * dst, uint32_t val);
extern void     ndz_write_U32_b(uint8_t * dst, uint32_t val);
extern void     ndz_write_I32_l(uint8_t * dst, int32_t  val);
extern void     ndz_write_I32_b(uint8_t * dst, int32_t  val);

extern int32_t  ndz_fread_U8str(FILE * fp, uint8_t  * ptr, int32_t n);
extern int32_t  ndz_fread_U16_l(FILE * fp, uint16_t * result);
extern int32_t  ndz_fread_U16_b(FILE * fp, uint16_t * result);
extern int32_t  ndz_fread_I16_l(FILE * fp, int16_t  * result);
extern int32_t  ndz_fread_I16_b(FILE * fp, int16_t  * result);
extern int32_t  ndz_fread_U32_l(FILE * fp, uint32_t * result);
extern int32_t  ndz_fread_U32_b(FILE * fp, uint32_t * result);
extern int32_t  ndz_fread_I32_l(FILE * fp, int32_t  * result);
extern int32_t  ndz_fread_I32_b(FILE * fp, int32_t  * result);

extern int32_t  ndz_fwrite_U8str(FILE * fp, const uint8_t * ptr, int32_t n);
extern int32_t  ndz_fwrite_U16_l(FILE * fp, uint16_t val);
extern int32_t  ndz_fwrite_U16_b(FILE * fp, uint16_t val);
extern int32_t  ndz_fwrite_I16_l(FILE * fp, int16_t  val);
extern int32_t  ndz_fwrite_I16_b(FILE * fp, int16_t  val);
extern int32_t  ndz_fwrite_U32_l(FILE * fp, uint32_t val);
extern int32_t  ndz_fwrite_U32_b(FILE * fp, uint32_t val);
extern int32_t  ndz_fwrite_I32_l(FILE * fp, int32_t  val);
extern int32_t  ndz_fwrite_I32_b(FILE * fp, int32_t  val);

#ifdef __cplusplus
}
#endif
#endif /* NADZUNA_FILE_IO_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
