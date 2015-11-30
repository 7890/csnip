/*
  Copyright (C) 2000 Paul Davis
  Copyright (C) 2003 Rohan Drape
  Copyright (C) 2015 Thomas Brand

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

  ISO/POSIX C version of Paul Davis's lock free ringbuffer C++ code.
  This is safe for the case of one read thread and one write thread.
*/

//tb/151126
/*
rb.h is derived from ringbuffer.c and ringhbuffer.h in jack repository
https://github.com/jackaudio/jack1

this file (rb.h) is part of a collection of c snipets
https://github.com/7890/csnip
refined for generic use (i.e. no JACK-specific naming), all-in-one .h file
allow arbitrary sized ringbuffers
	size must not necessariliy be a power of two
	buffer size will normally be exactly like requested
	if buffer size=n: _can_write==n after creation
methods and types renamed:
jack_ringbuffer_* -> rb_*
_create -> _new
_read_space -> _can_read, _write_space -> _can_write
_get_read_vector -> _get_read_vectors, _get_write_vector -> _get_write_vectors
_read_advance -> _advance_read_pointer, _write_advance -> _advance_write_pointer
advance methods return count (limited to can_read/can_write)
new methods:
_drop: advance read pointer up to write pointer
_peek_at: peek at offset
_get_next_read_vector, _get_next_write_vector
_find_byte, _find_byte_sequence
_read_byte, _peek_byte, _peek_byte_at, _skip_byte, _write_byte
alias:
_skip -> _advance_read_pointer
*/

//EXPERIMENTAL CODE
//NOT ALL METHODS FULLY TESTED

#ifndef _RB_H
#define _RB_H

#ifdef __cplusplus
extern "C" {
#endif

#define RB_USE_MLOCK 	/**< If defined, use POSIX memory locking. This flag is set by default.
			     You'd need to tweak the rb.h source file in order to disable the use of mlock() or make it a compile-time setting. */

#include <stdlib.h> //malloc, free
#include <string.h> //memcpy
#ifdef RB_USE_MLOCK
	#include <sys/mman.h> //mlock, munlock
#endif
#include <sys/types.h> //size_t

/** @file rb.h \mainpage
 * A set of library functions to work with lock-free ringbuffers.
 *
 * The key attribute of a ringbuffer is that it can be safely accessed
 * by two threads simultaneously -- one reading from the buffer and
 * the other writing to it -- without using any synchronization or
 * mutual exclusion primitives.  For this to work correctly, there can
 * only be a single reader and a single writer thread.  Their
 * identities cannot be interchanged.
 *
 * Please find a documentation of functions here: \ref rb.h.
 * 
 * rb.h is part of a collection of C snipets which can be found here: [https://github.com/7890/csnip](https://github.com/7890/csnip)
 */

/**
 * Ringbuffers are of type rb_t.
 * 
 * Example use:
 * @code
 * rb_t *ringbuffer;
 * ringbuffer=rb_new(1024);
 * if(ringbuffer!=NULL) { //do stuff }
 * @endcode
 */
typedef struct
{
  char *buffer;				/**< \brief Pointer to the linear flat byte buffer to hold all ringbuffer data.
					            The memory for the buffer will be allocated when rb_new() is called. */
  size_t size;				/**< \brief The size in bytes of the buffer as requested by caller. */
  volatile size_t read_pointer;		/**< \brief A pointer to the buffer for read operations. */
  volatile size_t write_pointer;	/**< \brief A pointer to the buffer for write operations. */
  int memory_locked;			/**< \brief Whether or not the buffer is locked to memory (if locked, no virtual memory disk swaps). */
  volatile int last_was_write;		/**< \brief Whether or not the last operation on the buffer was of type write (write pointer advanced).
					            !last_was_write corresponds to read operation accordingly (read pinter advanced). */
} 
rb_t;

/**
 * Read and write vectors are of type rb_data_t.
 * 
 * See rb_get_read_vector
 *
 * Example use:
 * @code
 * rb_data_t rvec[2];
 * rb_get_read_vectors(rb,rvec);
 * if(rvec[0].size>0) { //do stuff with rvec[0].buffer, first part of possible split }
 * if(rvec[1].size>0) { //do stuff with rvec[1].buffer, second part of possible split }
 * @endcode
 */
typedef struct  
{
  char  *buffer;	/**< \brief Pointer to location in main byte buffer. It correponds to a partial (segment)
			            or full area of the main byte buffer in rb_t. */
  size_t size;		/**< \brief Count of bytes that can be read from the buffer. */
} 
rb_data_t;

rb_t *rb_new(size_t size);
void rb_free(rb_t *rb);
int rb_mlock(rb_t *rb);
void rb_reset(rb_t *rb);

size_t rb_can_read(const rb_t *rb);
size_t rb_can_write(const rb_t *rb);

size_t rb_read(rb_t *rb, char *destination, size_t count);
size_t rb_write(rb_t *rb, const char *source, size_t count);

size_t rb_peek(const rb_t *rb, char *destination, size_t count);
size_t rb_peek_at(const rb_t *rb, char *destination, size_t count, size_t offset);

size_t rb_drop(rb_t *rb);

int rb_find_byte(rb_t *rb, char byte, size_t *offset);
int rb_find_byte_sequence(rb_t *rb, char *pattern, size_t pattern_offset, size_t count, size_t *offset);

size_t rb_read_byte(rb_t *rb, char *destination);
size_t rb_peek_byte(const rb_t *rb, char *destination);
size_t rb_peek_byte_at(const rb_t *rb, char *destination, size_t offset);
size_t rb_skip_byte(rb_t *rb);
size_t rb_write_byte(rb_t *rb, const char *source);

size_t rb_advance_read_pointer(rb_t *rb, size_t count);
size_t rb_advance_write_pointer(rb_t *rb, size_t count);

void rb_get_read_vectors(const rb_t *rb, rb_data_t *vec);
void rb_get_write_vectors(const rb_t *rb, rb_data_t *vec);
void rb_get_next_read_vector(const rb_t *rb, rb_data_t *vec);
void rb_get_next_write_vector(const rb_t *rb, rb_data_t *vec);

/*
rules in this ring:
-when imagined as a linear space with infinite length
	-read pointer can never pass write pointer
	-write pointer can never pass read pointer
	-read and write pointers can be at the same place

both r and w at the same position:
     r
..-------..
     w

ambiguous. possible meanings:
A) cannot write (0),  max read  (size)
B) cannot read  (0) , max write (size)
-> need to know last pointer movement

if last was w:

     r
..-------..
  >>>w
-> case A

if last was r:

  >>>r
..-------..
     w
->case B

after creation of a new ringbuffer r==w==0, last moved pointer r (can read 0, can write size)

any constellation of read and write pointers where r!=w: last pointer movement must not be considered

  r   w
----------
  ....
can read: w-r
can write: size-(w-r)

  w   r
----------
      .... size-r
.. + w
can read: w+size-r
can write r-w

further considerations
lock-free:
-two independent threads with mutually exclusive identities can access the (shared) ringbuffer 
 without using locking mechanisms such as mutex

non-blocking:
-the memory for the ringbuffer is initialized on creation and can't be changed during read and write operations
	hence no further memory allocation is involved during operation
-if the memory is locked with mlock, swaping operations are not expected
-read and write methods will return in a predictable time (?)

thread safety:
-one reader will never create a situation where two read operations are happening at the same time
	hence this won't happen:
	-two readers trying to move the read pointer at the same time
	-out-of-sequence situation: 
		reader a starts to read, reader b starts to read
			reader a finished, reader b finished OR
			reader b finished, reader a finished
-the same applies to the writing side

-thread safe read operations won't touch the write pointer or touch bytes beyond the write pointer
-thread safe write operations won't touch the read pointer or touch bytes beyond the read pointer

-methods that move read AND write pointers are all NOT thread safe
*/

/**
 * Allocates a ringbuffer data structure of a specified size. The
 * caller must arrange for a call to rb_free() to release
 * the memory associated with the ringbuffer after use.
 *
 * @param size the ringbuffer size in bytes, >0
 *
 * @return a pointer to a new rb_t, if successful; NULL
 * otherwise.
 */
//=============================================================================
rb_t *rb_new(size_t size)
{
	if(size<1) {return NULL;}

	rb_t *rb;
	
	rb=(rb_t*)malloc(sizeof(rb_t)); //
	if(rb==NULL) {return NULL;}

	rb->size=size;
	rb->write_pointer=0;
	rb->read_pointer=0;

	rb->buffer=(char*)malloc(rb->size); //
	if(rb->buffer==NULL)
	{
		free(rb);
		return NULL;
	}
	rb->memory_locked=0;
	rb->last_was_write=0;	

	return rb;
}

/**
 * Frees the ringbuffer data structure allocated by an earlier call to rb_new().
 *
 * This is not thread safe.
 *
 * @param rb a pointer to the ringbuffer structure.
 */
//=============================================================================
void rb_free(rb_t *rb)
{
	if(rb==NULL) {return;}
#ifdef RB_USE_MLOCK
	if(rb->memory_locked)
	{
		munlock(rb->buffer, rb->size);
	}
#endif
	rb->size=0;
	free(rb->buffer);
	free(rb);
}

/**
 * Lock a ringbuffer data block into memory.
 *
 * Uses the mlock() system call.
 * 
 * This is not a realtime operation.
 *
 * @param rb a pointer to the ringbuffer structure.
 */
//=============================================================================
int rb_mlock(rb_t *rb)
{
#ifdef RB_USE_MLOCK
	if(mlock(rb->buffer, rb->size)) {return -1;}
#endif
	rb->memory_locked=1;
	return 0;
}

/**
 * Reset the read and write pointers, making an empty buffer.
 *
 * This is not thread safe.
 *
 * @param rb a pointer to the ringbuffer structure.
 */
//=============================================================================
void rb_reset(rb_t *rb)
{
	rb->read_pointer=0;
	rb->write_pointer=0;
	rb->last_was_write=0;
}

// in front of the read pointer and behind the write pointer.
/**
 * Return the number of bytes available for reading.
 *
 * This is the number of bytes in front of the read pointer up to the write pointer.
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the number of bytes available to read.
 */
//=============================================================================
size_t rb_can_read(const rb_t *rb)
{
	size_t r=rb->read_pointer;
	size_t w=rb->write_pointer;

	if(r==w)
	{
		if(rb->last_was_write) {return rb->size;}
		else {return 0;}
	}
	else if(r<w) {return w-r;}
	else {return w+rb->size-r;} //r>w
}

/**
 * Return the number of bytes available for writing.
 *
 * This is the number of bytes in front of the write pointer up to the read pointer.
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the amount of free space (in bytes) available for writing.
 */
//=============================================================================
size_t rb_can_write(const rb_t *rb)
{
	size_t r=rb->read_pointer;
	size_t w=rb->write_pointer;

	if(r==w)
	{
		if(rb->last_was_write) {return 0;}
		else {return rb->size;}
	}
	else if(r<w) {return rb->size-w+r;}
	else {return r-w;} //r>w
}

/**
 * Read data from the ringbuffer and move the read pointer accordingly.
 * This is a copying data reader.
 *
 * The count of bytes effectively read can be less than the requested
 * count, which is limited to rb_can_read() bytes.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a buffer where data read from the
 * ringbuffer will go.
 * @param count the number of bytes to read.
 *
 * @return the number of bytes read, which may range from 0 to count.
 */
//=============================================================================
size_t rb_read(rb_t *rb, char *destination, size_t count)
{
	if(count==0) {return 0;}
	size_t can_read_count;
	//can not read more than offset, no chance to read from there
	if(!(can_read_count=rb_can_read(rb))) {return 0;}
	size_t do_read_count=count>can_read_count ? can_read_count : count;
	size_t linear_end=rb->read_pointer+do_read_count;
	size_t copy_count_1;
	size_t copy_count_2;

	if(linear_end>rb->size)
	{
		copy_count_1=rb->size-rb->read_pointer;
		copy_count_2=linear_end-rb->size;
	}
	else
	{
		copy_count_1=do_read_count;
		copy_count_2=0;
	}

	memcpy(destination, &(rb->buffer[rb->read_pointer]), copy_count_1);

	if(!copy_count_2)
	{
		rb->read_pointer=(rb->read_pointer+copy_count_1) % rb->size;
	}
	else
	{
		memcpy(destination+copy_count_1, &(rb->buffer[0]), copy_count_2);
		rb->read_pointer=copy_count_2 % rb->size;
	}
	rb->last_was_write=0;
	return do_read_count;
}

/**
 * Write data into the ringbuffer and move the write pointer accordingly.
 * This is a copying data writer.
 *
 * The count of bytes effectively written can be less than the requested
 * count, which is limited to rb_can_write() bytes.
 * 
 * @param rb a pointer to the ringbuffer structure.
 * @param source a pointer to the data to be written to the ringbuffer.
 * @param count the number of bytes to write.
 *
 * @return the number of bytes written, which may range from 0 to count
 */
//=============================================================================
size_t rb_write(rb_t *rb, const char *source, size_t count)
{
	size_t can_write_count;
	size_t do_write_count;
	size_t linear_end;
	size_t copy_count_1;
	size_t copy_count_2;

	if(!(can_write_count=rb_can_write(rb)))
	{
		return 0;
	}

	do_write_count=count>can_write_count ? can_write_count : count;
	linear_end=rb->write_pointer+do_write_count;

	if(linear_end>rb->size)
	{
		copy_count_1=rb->size-rb->write_pointer;
		copy_count_2=linear_end-rb->size;
	}
	else
	{
		copy_count_1=do_write_count;
		copy_count_2=0;
	}

	memcpy(&(rb->buffer[rb->write_pointer]), source, copy_count_1);

	if(!copy_count_2)
	{
		rb->write_pointer=(rb->write_pointer+copy_count_1) % rb->size;
	}
	else
	{
		memcpy(&(rb->buffer[0]), source+copy_count_1, copy_count_2);
		rb->write_pointer=copy_count_2 % rb->size;
	}
	rb->last_was_write=1;
	return do_write_count;
}

/**
 * Read data from the ringbuffer. Opposed to rb_read()
 * this function does not move the read pointer. Thus it's
 * a convenient way to inspect data in the ringbuffer in a
 * continuous fashion. The data is copied into a buffer provided by the caller.
 * For "raw" non-copy inspection of the data in the ringbuffer 
 * use rb_get_read_vectors() or rb_get_next_read_vector().
 *
 * The count of bytes effectively read can be less than the requested
 * count, which is limited to rb_can_read() bytes.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a buffer where data read from the
 * ringbuffer will go.
 * @param count the number of bytes to read.
 *
 * @return the number of bytes read, which may range from 0 to count.
 */
//=============================================================================
size_t rb_peek(const rb_t *rb, char *destination, size_t count)
{
	return rb_peek_at(rb,destination,count,0);
}

/**
 * Read data from the ringbuffer. Opposed to rb_read()
 * this function does not move the read pointer. Thus it's
 * a convenient way to inspect data in the ringbuffer in a
 * continuous fashion. The data is copied into a buffer provided by the caller.
 * For "raw" non-copy inspection of the data in the ringbuffer 
 * use rb_get_read_vectors() or rb_get_next_read_vector().
 *
 * The count of bytes effectively read can be less than the requested
 * count, which is limited to rb_can_read() bytes.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a buffer where data read from the
 * ringbuffer will go.
 * @param count the number of bytes to read.
 * @param offset the number of bytes to skip at the start of readable ringbuffer data.
 *
 * @return the number of bytes read, which may range from 0 to count.
 */
//=============================================================================
size_t rb_peek_at(const rb_t *rb, char *destination, size_t count, size_t offset)
{
	if(count==0) {return 0;}
	size_t can_read_count;
	//can not read more than offset, no chance to read from there
	if((can_read_count=rb_can_read(rb))<=offset) {return 0;}
	//limit read count respecting offset
	size_t do_read_count=count>can_read_count-offset ? can_read_count-offset : count;
	//adding the offset, knowing it could be beyond buffer end
	size_t tmp_read_pointer=rb->read_pointer+offset;
	//including all: current read pointer + offset + limited read count
	size_t linear_end=tmp_read_pointer+do_read_count;
	size_t copy_count_1;
	size_t copy_count_2;

	//beyond
	if(linear_end>rb->size)
	{
		//still beyond
		if(tmp_read_pointer>=rb->size)
		{
			//all in rolled over
			tmp_read_pointer%=rb->size;
			copy_count_1=do_read_count;
			copy_count_2=0;
		}
		//segmented
		else
		{
			copy_count_1=rb->size-tmp_read_pointer;
			copy_count_2=linear_end-rb->size-offset;
		}
	}
	else
	//if not beyond the buffer end
	{
		copy_count_1=do_read_count;
		copy_count_2=0;
	}

	memcpy(destination, &(rb->buffer[tmp_read_pointer]), copy_count_1);

	if(copy_count_2)
	{
		memcpy(destination+copy_count_1, &(rb->buffer[0]), copy_count_2);
	}
	return do_read_count;
}

/**
 * Drop / ignore all data available to read.
 * This moves the read pointer up to the current write pointer
 * (nothing left to read) using rb_advance_read_pointer().
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the number of bytes dropped, which may range from 0 to rb->size.
 */
//=============================================================================
size_t rb_drop(rb_t *rb)
{
	return rb_advance_read_pointer(rb,rb_can_read(rb));
}

/**
 * Search for a given byte in the ringbuffer's readable space.
 * The index at which the byte was found is copied to the
 * offset variable provided by the caller. The index is relative to
 * the start of the ringbuffer's readable space.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param byte the byte to search in the ringbuffer's readable space
 * @param offset a pointer to a size_t variable
 *
 * @return success: found==1, not found==0
 */
//=============================================================================
int rb_find_byte(rb_t *rb, char byte, size_t *offset)
{
	size_t off=0;
	char c;
	while(rb_peek_byte_at(rb,&c,off))
	{
		if(c==byte)
		{
			memcpy(offset,&(off),sizeof(size_t)); //hmmm.
			return 1;
		}
		off++;
	}
	off=0;
	memcpy(offset,&(off),sizeof(size_t));
	return 0;
}

/**
 * Read one byte from the ringbuffer.
 * This advances the read pointer by one byte if at least one byte
 * is available in the ringbuffer's readable space.
 *
 * This is a copying data reader.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a variable where the byte read from the
 * ringbuffer will go.
 *
 * @return the number of bytes read, which may range from 0 to 1.
 */
//=============================================================================
size_t rb_read_byte(rb_t *rb, char *destination)
{
	return rb_read(rb,destination,1);
}
/**
 * Peek one byte from the ringbuffer (don't move the read pointer).
 * This is a copying data reader.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param destination a pointer to a variable where the byte read from the
 * ringbuffer will go.
 *
 * @return the number of bytes read, which may range from 0 to 1.
 */
//=============================================================================
size_t rb_peek_byte(const rb_t *rb, char *destination)
{
	return rb_peek_byte_at(rb,destination,0);
}

/**
* n/a
*/
//=============================================================================
size_t rb_peek_byte_at(const rb_t *rb, char *destination, size_t offset)
{
	size_t can_read_count;
	if((can_read_count=rb_can_read(rb)<=offset)) {return 0;}

	size_t tmp_read_pointer=rb->read_pointer+offset;

	if(rb->size<=tmp_read_pointer)
	{
		memcpy(destination, &(rb->buffer[tmp_read_pointer-rb->size]),1);
	}
	else
	{
		memcpy(destination, &(rb->buffer[tmp_read_pointer]),1);
	}
}

/**
 * Drop / ignore one byte available to read.
 * This advances the read pointer by one byte if at least one byte
 * is available in the ringbuffer's readable space.
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the number of bytes skipped, which may range from 0 to 1.
 */
//=============================================================================
size_t rb_skip_byte(rb_t *rb)
{
	return rb_advance_read_pointer(rb,1);
}

/**
 * Write one byte to the ringbuffer.
 * This advances the write pointer by one byte if at least one byte
 * can be written to the ringbuffer's writable space.
 *
 * @param rb a pointer to the ringbuffer structure.
 *
 * @return the number of bytes written, which may range from 0 to 1.
 */
//=============================================================================
size_t rb_write_byte(rb_t *rb, const char *source)
{
	return rb_write(rb,source,1);
}

/**
 * Advance the read pointer.
 *
 * After data have been read from the ringbuffer using the pointers
 * returned by rb_get_read_vectors() or rb_get_next_read_vector(), use this
 * function to advance the read pointer, making that space available for 
 * future write operations.
 *
 * The count of the read pointer advance can be less than the requested
 * count, which is limited to rb_can_read() bytes.
 *
 * Advancing the read pointer without prior reading the involved bytes 
 * means dropping/ignoring data.
 * 
 * @param rb a pointer to the ringbuffer structure.
 * @param count the number of bytes to advance.
 *
 * @return the number of bytes advanced, which may range from 0 to count.
 */
//=============================================================================
size_t rb_advance_read_pointer(rb_t *rb, size_t count)
{
	if(count==0) {return 0;}
	size_t can_read_count;
	if(!(can_read_count=rb_can_read(rb))) {return 0;}

	size_t do_advance_count=count>can_read_count ? can_read_count : count;
	size_t r=rb->read_pointer;
	size_t linear_end=r+do_advance_count;
	size_t tmp_read_pointer=linear_end>rb->size ? linear_end-rb->size : r+do_advance_count;

	rb->read_pointer=(tmp_read_pointer%=rb->size);//
	rb->last_was_write=0;//
	return do_advance_count;
}

/**
 * Advance the write pointer.
 *
 * After data have been written the ringbuffer using the pointers
 * returned by rb_get_write_vectors() or rb_get_next_write_vector() use this
 * function to advance the write pointer, making the data available for
 * future read operations.
 *
 * The count of the write pointer advance can be less than the requested
 * count, which is limited to rb_can_write() bytes.
 *
 * Advancing the write pointer without prior writing the involved bytes 
 * means leaving arbitrary data in the ringbuffer.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param count the number of bytes to advance.
 *
 * @return the number of bytes advanced, which may range from 0 to count
 */
//=============================================================================
size_t rb_advance_write_pointer(rb_t *rb, size_t count)
{
	if(count==0) {return 0;}
	size_t can_write_count;
	if(!(can_write_count=rb_can_write(rb))) {return 0;}

	size_t do_advance_count=count>can_write_count ? can_write_count : count;
	size_t w=rb->write_pointer;
	size_t linear_end=w+do_advance_count;
	size_t tmp_write_pointer=linear_end>rb->size ? linear_end-rb->size : w+do_advance_count;

	rb->write_pointer=(tmp_write_pointer%=rb->size);//
	rb->last_was_write=1;//
	return do_advance_count;
}

/**
 * Fill a data structure with a description of the current readable
 * data held in the ringbuffer. This description is returned in a two
 * element array of rb_data_t. Two elements are needed
 * because the data to be read may be split across the end of the
 * ringbuffer.
 *
 * The first element will always contain a valid @a len field, which
 * may be zero or greater. If the @a len field is non-zero, then data
 * can be read in a contiguous fashion using the address given in the
 * corresponding @a buf field.
 *
 * If the second element has a non-zero @a len field, then a second
 * contiguous stretch of data can be read from the address given in
 * its corresponding @a buf field.
 * 
 * This method allows to read data from the ringbuffer directly using
 * pointers to the ringbuffer's readable spaces.
 *
 * If data was read this way, the caller must manually advance the read
 * pointer accordingly using rb_advance_read_pointer().
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param vec a pointer to a 2 element array of rb_data_t.
 *
 */
//=============================================================================
void rb_get_read_vectors(const rb_t *rb, rb_data_t *vec)
{
	size_t can_read_count=rb_can_read(rb);
	size_t r=rb->read_pointer;
	size_t linear_end=r+can_read_count;

	if(linear_end>rb->size)
	{
		// Two part vector: the rest of the buffer after the current write
		// pointer, plus some from the start of the buffer.
		vec[0].buffer=&(rb->buffer[r]);
		vec[0].size=rb->size-r;
		vec[1].buffer=rb->buffer;
		vec[1].size=linear_end-rb->size;
	}
	else
	{
		// Single part vector: just the rest of the buffer
		vec[0].buffer=&(rb->buffer[r]);
		vec[0].size=can_read_count;
		vec[1].size=0;
	}
}

/**
 * Fill a data structure with a description of the current writable
 * space in the ringbuffer. The description is returned in a two
 * element array of rb_data_t. Two elements are needed
 * because the space available for writing may be split across the end
 * of the ringbuffer.
 *
 * The first element will always contain a valid @a len field, which
 * may be zero or greater. If the @a len field is non-zero, then data
 * can be written in a contiguous fashion using the address given in
 * the corresponding @a buf field.
 *
 * If the second element has a non-zero @a len field, then a second
 * contiguous stretch of data can be written to the address given in
 * the corresponding @a buf field.
 *
 * This method allows to write data to the ringbuffer directly using
 * pointers to the ringbuffer's writable spaces.
 *
 * If data was written this way, the caller must manually advance the write
 * pointer accordingly using rb_advance_write_pointer().
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param vec a pointer to a 2 element array of rb_data_t.
 */
//=============================================================================
void rb_get_write_vectors(const rb_t *rb, rb_data_t *vec)
{
	size_t can_write_count=rb_can_write(rb);
	size_t w=rb->write_pointer;
	size_t linear_end=w+can_write_count;

	if(linear_end>rb->size)
	{
		// Two part vector: the rest of the buffer after the current write
		// pointer, plus some from the start of the buffer.
		vec[0].buffer=&(rb->buffer[w]);
		vec[0].size=rb->size-w;
		vec[1].buffer=rb->buffer;
		vec[1].size=linear_end-rb->size;
	}
	else
	{
		// Single part vector: just the rest of the buffer
		vec[0].buffer=&(rb->buffer[w]);
		vec[0].size=can_write_count;
		vec[1].size=0;
	}
}

/**
* n/a
*/
//=============================================================================
void rb_get_next_read_vector(const rb_t *rb, rb_data_t *vec)
{
	size_t can_read_count=rb_can_read(rb);
	size_t r=rb->read_pointer;
	size_t linear_end=r+can_read_count;

	if(linear_end>rb->size)
	{
		vec->buffer=&(rb->buffer[r]);
		vec->size=rb->size-r;
	}
	else
	{
		vec->buffer=&(rb->buffer[r]);
		vec->size=can_read_count;
	}
}

/**
* n/a
*/
//=============================================================================
void rb_get_next_write_vector(const rb_t *rb, rb_data_t *vec)
{
	size_t can_write_count=rb_can_write(rb);
	size_t w=rb->write_pointer;
	size_t linear_end=w+can_write_count;

	if(linear_end>rb->size)
	{
		vec->buffer=&(rb->buffer[w]);
		vec->size=rb->size-w;
	}
	else
	{
		vec->buffer=&(rb->buffer[w]);
		vec->size=can_write_count;
	}
}

/**
 * Search for a given byte sequence in the ringbuffer's readable space.
 * The index at which the byte sequence was found is copied to the
 * offset variable provided by the caller. The index is relative to
 * the start of the ringbuffer's readable space.
 *
 * @param rb a pointer to the ringbuffer structure.
 * @param pattern the byte sequence to search in the ringbuffer's readable space
 * @param pattern_offset the offset to apply to the byte sequence
 * @param count the count of bytes to consider for matching in the byte sequence, starting from offset
 * @param offset a pointer to a size_t variable
 *
 * @return success: found==1, not found==0
 */
//=============================================================================
int rb_find_byte_sequence(rb_t *rb, char *pattern, size_t pattern_offset, size_t count, size_t *offset)
{
	char *tmp_pattern=pattern;
	tmp_pattern+=pattern_offset;

	char compare_buffer[count];
	size_t off=0;
	char c;
	while(rb_peek_byte_at(rb,&c,off))
	{
		if(c==tmp_pattern[0])//found start
		{
			//read to compare buffer
			if(rb_peek_at(rb,compare_buffer,count,off)==count)
			{
				if(!memcmp(tmp_pattern, compare_buffer,count))
				{
					memcpy(offset,&(off),sizeof(size_t)); //hmmm.
					return 1;
				}
			}
			else {goto _not_found;}
		}
		off++;
	}
_not_found:
	off=0;
	memcpy(offset,&(off),sizeof(size_t));
	return 0;
}

//=============================================================================
//"ALIASES"

/**
* n/a
*/
size_t rb_skip(rb_t *rb, size_t count) {return rb_advance_read_pointer(rb,count);}

//#define ALIASES_1
#ifdef ALIASES_1
//if rb.h is used as a jack_ringbuffer replacement these wrappers simplify source modification
//(sed 's/jack_ringbuffer_/rb_/g')
rb_t *rb_create(size_t size)			{return rb_new(size);}
size_t rb_read_space(const rb_t *rb)		{return rb_can_read(rb);}
size_t rb_write_space(const rb_t *rb)		{return rb_can_write(rb);}
size_t rb_read_advance(rb_t *rb, size_t count)	{return rb_advance_read_pointer(rb,count);}
size_t rb_write_advance(rb_t *rb, size_t count)	{return rb_advance_write_pointer(rb,count);}
void rb_get_read_vector(const rb_t *rb, rb_data_t *vec) {return rb_get_read_vectors(rb, vec);}
void rb_get_write_vector(const rb_t *rb, rb_data_t *vec) {return rb_get_read_vectors(rb, vec);}
#endif

//#define ALIASES_2
#ifdef ALIASES_2
//inspired by https://github.com/xant/libhl/blob/master/src/rbuf.c,
//http://svn.drobilla.net/lad/trunk/raul/raul/RingBuffer.hpp
size_t rb_size(rb_t *rb)		{return rb->size;}
size_t rb_capacity(rb_t *rb)		{return rb->size;}
void rb_clear(rb_t *rb)			{rb_reset(rb);}
void rb_destroy(rb_t *rb)		{rb_free(rb);}
#endif

#ifdef __cplusplus
}
#endif

#endif //header guard
//EOF
