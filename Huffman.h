#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "HuffmanInternal.h"

namespace Huffman{
	enum Status{
		BufferTooSmall,
		InvalidInput,
		Done
	};


	/* Takes the input data and compresses it into the output buffer.If the buffer size is smaller than the 
	 * compressed data, the function will return BufferTooSmall
	 */
	Status compress(const uint8_t*data,int size,uint8_t*compress_buffer,int&buffer_size);


	/* Takes the input data and decompresses it into the output buffer.If the buffer size is smaller than the 
	 * uncompressed data,the function will return BufferTooSmall
	 * If the input data is not valid it will return InvalidInput
	 */
	Status decompress(const uint8_t*data,int size,uint8_t*decompress_buffer,int&buffer_size);

	/* Will return uncompressed size of a compressed buffer.
	 * !!!WARNING!!! Giving it an invalid input buffer is undefined behaviour as the function just reads 
	 * the first 4 bytes of the buffer
	 */
	unsigned int getUncompressedSize(const uint8_t*compressed);

}

#endif
