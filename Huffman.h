#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "HuffmanInternal.h"

namespace Huffman{
	enum Status{
		BufferTooSmall,
		InvalidInput,
		Done
	};
	

	Status compress(const uint8_t*data,int size,uint8_t*compress_buffer,int&buffer_size);
	Status decompress(const uint8_t*data,int size,uint8_t*decompress_buffer,int&buffer_size);
	unsigned int getUncompressedSize(const uint8_t*compressed);

}

#endif
