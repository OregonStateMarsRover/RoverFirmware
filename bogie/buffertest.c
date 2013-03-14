/*
 *	Ring Buffer Test
 * 	
 *	Purpose: To test the functionality of the ring buffer
 *	on a computer (where it's easy to debug) before trusting
 *	it on an embedded controller (where it isn't).
 *	
 * 	Author:
 *	Marshal Horn
 */

#include <stdio.h>
#include "RingBuffer.h"


void print_buffer( RingBuffer * buf ) {
	fprintf(stdout, "{" );
	int i;
	for(i = 0; i < buf->size; ++i) {
		int index = (i + buf->start) % buf->cap;
		fprintf(stdout, "%d, ", buf->data[index]);
	}
	fprintf(stdout, "}\n" ); 
}

void buffer_stats( RingBuffer * buf ) {
	fprintf(stdout, "Cap: %d	Used: %d	Free: %d	Err: %X\n",
		buf->cap, RingBufferBytesUsed( buf ),
		RingBufferBytesFree( buf ), buf->err );
}

int compare( char is, char * list ) {
	if( !is ) {	// if is == NULL
		return 1;
	}
	for( ; *list; ++list ) {	// while not NULL
		if( is == *list ) {
			return 1;
		}
	}
	return 0;
}
int getnum( char *string, int * index ) {
	int i = *index;

	/* Find the start of the number */
	for( ; !(string[i] >= '0' 
				&& string[i] <= '9'); ++i)
	{
		if( compare( string[i], "\r\n" ) )
			*index = i;
			printf("Aborted search at index %d.\n", i );
			return 0;
	}

	/* Now decode the number */
	printf("Started decoding number at index %d.\n", i );
	int sum;
	for( sum = 0; (string[i] <= '9' && string[i] >= '0' ); ++i) {
		sum = sum * 10 + string[i] - '0';
	}
	*index = i;
	return sum;
}

int testbuf( int size ) {

	RingBuffer * mybuf = malloc(sizeof(RingBuffer));;
	RingBufferInit( mybuf, size );

	char input[100];
	unsigned char loop = 1;
	int num;

	while( loop ) {
		fgets( input, 100, stdin );
		int index = 0;
		switch(input[index++]) {	// return 0, but increment to 1
			case 'l':
				// echo the list
				print_buffer( mybuf );
				break;
			case 'i':
				// insert an element into the list
				num = getnum( input, &index );
				RingBufferAddByte( mybuf, num );
				break;
			case 'd':
				// delete an element from the list
				fprintf(stdout, "Removed %d\n", 
						RingBufferGetByte( mybuf ) );
				break;

			case 'I':
				// insert multiple elements in the list

				break;

			case 'D':
				// delete multiple elements from the list
				break;

			case 'q':
				// quit the program
				loop = 0;
				break;

			case '?':
				// print options
				fprintf(stderr, "%s%s%s%s%s%s%s",
						"Options:\n",
						"l shows the current items in the list.\n",
						"i inserts a single element.\n",
						"I inserts multiple elements.\n",
						"d deletes a single element.\n",
						"D deletes multiple elements.\n",
						"q quits the program.\n");
				break;
			default:
				break;
		}
		buffer_stats( mybuf );
	}
	RingBufferDelete( mybuf );
	free( mybuf ); 
		
	return 0;
}


int main( int argc, char **argv ) {
	unsigned short size;
	if( argc == 2 ) {
		size = atoi( argv[1] );
	} else {
		size = 10;
	}
	
//	testbuf( size );
	
	char input[100];
	fgets( input, 100, stdin );
	int idx = 0;

	int num = getnum( input, &idx );
	

	printf("The number you entered was %d, ending at index %d.\n", 
		num, idx );


	return 0;
}
