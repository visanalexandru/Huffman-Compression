#include "Huffman.h"

namespace Huffman{

	Status compress(const uint8_t*data,int size,uint8_t*compress_buffer,int&buffer_size){



		HuffmanInternal::buffer paths[256];
		HuffmanInternal::node*root=HuffmanInternal::createTree(data,size);

		if(!root->isLeaf){
			assignPaths(root,paths,"");
		}
		else {
			paths[(int)data[0]]=HuffmanInternal::buffer("0");	
		}

		HuffmanInternal::buffer compressed,tree;

		for(int i=0;i<size;i++){
			compressed.addBuffer(paths[(int)data[i]]);
		}

		saveTree(root,tree);
		int total_size=compressed.data.size()+tree.data.size()+3*sizeof(uint32_t);		
		if(buffer_size<total_size)
			return Status::BufferTooSmall;

		buffer_size=total_size;
		HuffmanInternal::addInt(size,compress_buffer);
		HuffmanInternal::addInt(compressed.write_cursor,compress_buffer+sizeof(uint32_t));
		HuffmanInternal::addInt(tree.write_cursor,compress_buffer+2*sizeof(uint32_t));

		HuffmanInternal::addBuffer(compressed,compress_buffer+3*sizeof(uint32_t));
		HuffmanInternal::addBuffer(tree,compress_buffer+3*sizeof(uint32_t)+compressed.data.size());



		delete root;
		return Status::Done;
	}
	unsigned int getUncompressedSize(const uint8_t*compressed){
		return HuffmanInternal::readInt(compressed);
	}
	Status decompress(const uint8_t*data,int size,uint8_t*decompress_buffer,int&buffer_size){

		if(size<3*sizeof(uint32_t))
			return Status::InvalidInput;

		unsigned int total_size=getUncompressedSize(data);
		if(total_size>buffer_size)
			return Status::BufferTooSmall;


		unsigned int comp_bits=HuffmanInternal::readInt(data+sizeof(uint32_t));
		unsigned int tree_bits=HuffmanInternal::readInt(data+2*sizeof(uint32_t));

		unsigned int comp_size=HuffmanInternal::bitsToBytes(comp_bits);
		unsigned int tree_size=HuffmanInternal::bitsToBytes(tree_bits);

		if(size<comp_size+tree_size+3*sizeof(uint32_t))
			return Status::InvalidInput;

		HuffmanInternal::buffer compressed=HuffmanInternal::readBuffer(data+3*sizeof(uint32_t),comp_bits);
		HuffmanInternal::buffer tree=HuffmanInternal::readBuffer(data+3*sizeof(uint32_t)+comp_size,tree_bits);	



		HuffmanInternal::node*root=loadTree(tree);
		HuffmanInternal::decodeHuffman(compressed,decompress_buffer,root);
		delete root;

		return Status::Done;
	}


}
