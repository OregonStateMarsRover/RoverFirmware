/**********************************************************************
 *
 * Basic C ring buffer. Designed to be portable between architectures
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************************/

#include <stdlib.h> // for malloc()
#include <string.h> // for memcpy()
#include <avr/interrupt.h> //for sei() and cli()

#include "RingBuffer.h"


void RingBufferInit(RingBuffer * buf, unsigned short capacity) {
	buf->data = (unsigned char*)malloc(sizeof(unsigned char)*capacity);

	buf->start = 0;
	buf->size= 0;
	buf->cap = capacity;
	buf->err = 0;	// start off with no error
}

void RingBufferDelete(RingBuffer * buf) {
	free(buf->data);

	buf->data = 0;
	buf->start = 0;
	buf->size = 0;
	buf->cap = 0;
}

unsigned short RingBufferAdd(RingBuffer * buf, unsigned char * data, 
		unsigned short length) {
	cli();

	/* If we try to put in too many elements, then don't insert any. */
	if( (length + buf->size) > buf->cap ) {
		buf->err |= LONG_MSG_ERR;
		length = 0;	// wrote zero bytes
	} else {
		unsigned short end = (buf->start + buf->size) % buf->cap;
		/* We are using some pointer arithmatic here.
		 * Don't worry - our data length is one byte.
		 * I only wrote it this way because it looks simpler than
		 * indexing and then referencing.
		 */
		if( (end + length) > buf->cap ) {
			memcpy( (buf->data + end) , data, (buf->cap - end) );
			memcpy( buf->data, (data + buf->cap - end), 
					(length + end - buf->cap) );
		} else {
			memcpy( &(buf->data[end]), data, length );
		}
		buf->size += length;
	}
	sei();
	return length;
}

bool RingBufferAddByte(RingBuffer * buf, unsigned char data) {
	cli();
	unsigned char rval;
	if( buf->size < buf->cap ) {
		buf->data[ (buf->size + buf->start) % buf->cap ] = data;
		buf->size++;

		rval = true;
	} else {
		buf->err |= OVERFULL_ERR;
		rval = false; // buffer full, failed to add
	}

	sei();
	return rval;
}

unsigned char RingBufferGetByte(RingBuffer * buf) {
	cli();
	unsigned char rval;
	if (buf->size) {
		unsigned char data = buf->data[buf->start];
		buf->start = (buf->start+1) % buf->cap;
		buf->size--;
		rval = data;
	} else {
		buf->err |= READ_EMPTY_ERR;
		rval = 0;
	}
	sei();
	return rval;
}

unsigned short RingBufferGetData(RingBuffer * buf, unsigned char * dest_buf, unsigned short bytes) {
	cli();

	if( bytes > buf->size ) {
		bytes = buf->size;
	}

	/* We are using some pointer arithmatic here.
	 * Don't worry - our data length is one byte.
	 * I only wrote it this way because it looks simpler than
	 * indexing and then referencing.
	 */
	if( (buf->start + bytes ) > buf->cap ) {
		memcpy( dest_buf, (buf->data + buf->start) , (buf->cap - buf->start) );
		memcpy( (dest_buf + buf->cap - buf->start), buf->data, 
				(bytes + buf->start - buf->cap) );
	} else {
		memcpy( dest_buf, &(buf->data[buf->start]), bytes );
	}

	buf->start = (buf->start + bytes) % buf->cap;
	buf->size -= bytes;
	sei();
	return bytes;
}

unsigned short RingBufferBytesUsed(RingBuffer * buf) {
	return buf->size;
}

unsigned short RingBufferBytesFree(RingBuffer * buf) {
	return buf->cap - buf->size;
}

unsigned char RingBufferClearError( RingBuffer * buf, unsigned char err_mask ) {
	buf->err &= ~err_mask;
	return buf->err;
}
