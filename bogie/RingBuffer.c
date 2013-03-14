/**********************************************************************
 *
 * Basic C ring buffer. Designed to be portable between architectures
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************************/

#include <stdlib.h>

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
	unsigned short i;
	// TODO When I'm more awake: Don't use a stupid loop. Use memcpy.
	for (i=0;i< length; i++) {
		if (!RingBufferAddByte(buf, data[i]))
			break;
	}

	return i;
}

bool RingBufferAddByte(RingBuffer * buf, unsigned char data) {
	if( buf->size < buf->cap ) {
		buf->size++;
		
		buf->data[ (buf->size + buf->start) % buf->cap ] = data;

		return true;
	}
	// else
	buf->err |= OVERFULL_ERR;
	return false; // buffer full, failed to add
}

unsigned char RingBufferGetByte(RingBuffer * buf) {
	if (buf->size) {
		unsigned char data = buf->data[buf->start];
		buf->start = (buf->start+1) % buf->size;
		buf->size--;
		return data;
	}
	// else
	buf->err |= READ_EMPTY_ERR;
	return 0;
}

unsigned short RingBufferGetData(RingBuffer * buf, unsigned char * dest_buf, unsigned short bytes) {
	unsigned short i, toRead;
	// TODO When I'm more awake: Don't use a stupid loop. Use memcpy.

	if (RingBufferBytesUsed(buf) < bytes) {
		toRead = RingBufferBytesUsed(buf);
	}
	else {
		toRead = bytes;
	}

	for (i=0;i<toRead;i++) {
		dest_buf[i] = RingBufferGetByte(buf);
	}

	return toRead; 
}

unsigned short RingBufferBytesUsed(RingBuffer * buf) {
	return buf->size;
}

unsigned short RingBufferBytesFree(RingBuffer * buf) {
	return buf->cap - buf->size;
}

