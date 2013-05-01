/**********************************************************************
 *
 * Basic C ring buffer. Designed to be portable between architectures
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************************/

#ifndef HAVE_RINGBUFFER_H
#define HAVE_RINGBUFFER_H

#define OVERFULL_ERR 0x01
#define READ_EMPTY_ERR 0x02
#define LONG_MSG_ERR 0x10

#ifndef bool
#define bool unsigned char
#define true 1
#define false 0
#endif


/* When the buffer is not large enough to hold all that you want to write, it will write all that it can (without overwriting), and then set the overflow error.
 * However, if you're using the write single byte functions, then it will skip that entirely (since a partial message doesn't do any good), and set an error.
At some point I may make this dynamically sized, but I don't think that's very safe on an embedded device. */

struct RingBuffer {
	unsigned char * data;	// pointer to array
	unsigned short start;	// start index
	unsigned short size;	// number of elements filled
	unsigned short cap;		// number of bytes allocated
	unsigned char err;		// check this to see if there has been a problem
};

typedef struct RingBuffer RingBuffer;

void RingBufferInit(RingBuffer * buf, unsigned short capacity);
void RingBufferDelete(RingBuffer * buf);
unsigned short RingBufferAdd(RingBuffer * buf, unsigned char * data, unsigned short length);
bool RingBufferAddByte(RingBuffer * buf, unsigned char data);
unsigned char RingBufferGetByte(RingBuffer * buf);
unsigned short RingBufferGetData(RingBuffer * buf, unsigned char * dest_buf, unsigned short bytes);
unsigned short RingBufferBytesUsed(RingBuffer * buf);
unsigned short RingBufferBytesFree(RingBuffer * buf);
unsigned char RingBufferClearError(RingBuffer * buf, unsigned char err_mask );

#endif

