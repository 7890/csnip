/** @file rb_float.h \mainpage
 * rb_float.h is part of a collection of C snippets which can be found here:
 * [https://github.com/7890/csnip](https://github.com/7890/csnip)
 *
 * Copyright (C) 2015 Thomas Brand
 */

#ifndef _RB_FLOAT_H
#define _RB_FLOAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rb.h"

static inline size_t rb_read_float(rb_t *rb, float *destination);
static inline size_t rb_peek_float(rb_t *rb, float *destination);
static inline size_t rb_peek_float_at(rb_t *rb, float *destination, size_t offset);
static inline size_t rb_skip_float(rb_t *rb);
static inline size_t rb_write_float(rb_t *rb, const float *source);

/**
 * Read sizeof(float) bytes from the ringbuffer.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a variable where the float value read from the
 * ringbuffer will be copied to.
 *
 * @return the number of bytes read, which may be 0 or sizeof(float).
 */
//=============================================================================
static inline size_t rb_read_float(rb_t *rb, float *destination)
{
	size_t can_read=rb_can_read(rb);
	if(can_read>=sizeof(float))
	{
		return rb_read(rb,(char*)destination,sizeof(float));
	}
	rb->total_underflows++;
	return 0;
}

/**
 * Read sizeof(float) bytes from the ringbuffer. Opposed to rb_read_float()
 * this function does not move the read index.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a variable where the float value read from the
 * ringbuffer will be copied to.
 *
 * @return the number of bytes read, which may be 0 or sizeof(float).
 */
//=============================================================================
static inline size_t rb_peek_float(rb_t *rb, float *destination)
{
	if(rb_can_read(rb)>=sizeof(float))
	{
		return rb_peek(rb,(char*)destination,sizeof(float));
	}
	rb->total_underflows++;
	return 0;
}

/**
 * Read sizeof(float) bytes from the ringbuffer at a given offset. Opposed to
 * rb_read_float() this function does not move the read index.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a variable where the float value read from the
 * ringbuffer will be copied to.
 * @param offset the number of bytes to skip at the start of readable ringbuffer data.
 *
 * @return the number of bytes read, which may be 0 or sizeof(float).
 */
//=============================================================================
static inline size_t rb_peek_float_at(rb_t *rb, float *destination, size_t offset)
{
	size_t can_read_count;
	if((can_read_count=rb_can_read(rb))<offset+sizeof(float))
	{
		rb->total_underflows++;
		return 0;
	}

	return rb_peek_at(rb,(char*)destination,sizeof(float),offset);
}

/**
 * Move the read index by sizeof(float) bytes.
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the number of bytes skipped, which may be 0 or sizeof(float).
 */
//=============================================================================
static inline size_t rb_skip_float(rb_t *rb)
{
	if(rb_can_read(rb)>=sizeof(float))
	{
		return rb_advance_read_index(rb,sizeof(float));
	}
	rb->total_underflows++;
	return 0;
}

/**
 * Write sizeof(float) bytes to the ringbuffer.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param source a pointer to the variable containing the float value to be written
 * to the ringbuffer.
 *
 * @return the number of bytes written, which may be 0 or sizeof(float).
 */
//=============================================================================
static inline size_t rb_write_float(rb_t *rb, const float *source)
{
	if(rb_can_write(rb)>=sizeof(float))
	{
		return rb_write(rb,(char*)source,sizeof(float));
	}
	rb->total_overflows++;
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif //header guard
//EOF
