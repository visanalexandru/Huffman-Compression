#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<vector>
#include<queue>
#include<stdio.h>
#include<string.h>
#include<iostream>

namespace Huffman{
	enum Status{
		BufferTooSmall,
		InvalidInput,
		Done
	};
	struct node{
		node*left,*right;
		uint8_t data;
		int cost;
		bool isLeaf;


		node(uint8_t byte,int value):left(nullptr),
		right(nullptr),
		data(byte),
		cost(value),
		isLeaf(true)
		{

		}

		node(node*l,node*r):left(l),
		right(r),
		data(' '),
		cost(l->cost+r->cost),
		isLeaf(false){

		}
		~node(){
			delete left;
			delete right;
		}
	};

	struct heapCompare{

		bool operator ()(node*a,node*b){
			return a->cost>b->cost;
		}

	};



	struct buffer{
		std::vector<uint8_t> data;
		int write_cursor;
		int read_cursor;

		buffer():write_cursor(0),read_cursor(0){

		}
		buffer(const std::string&data):write_cursor(0),read_cursor(0){
			for(const char&a:data){
				if(a=='1')
					addBit(true);
				else if(a=='0')
					addBit(false);
			}
		}
		bool reachedEnd(){
			return write_cursor==read_cursor;
		}
		void addBit(bool bit){

			if(write_cursor%8==0)
				data.push_back(0);

			if(bit)
				data[write_cursor/8]|=(1<<(write_cursor%8));

			write_cursor++;
		}	
		void addByte(uint8_t byte){
			if(write_cursor%8==0){
				data.push_back(byte);
			}
			else{
				int c=write_cursor%8;
				uint8_t r=byte>>(8-c);
				uint8_t l=byte<<c;

				data.push_back(0);

				data[write_cursor/8]|=l;
				data[write_cursor/8+1]|=r;
			}
			write_cursor+=8;
		}
		void addBuffer(buffer&to_add){
			int last=to_add.read_cursor;
			while(to_add.read_cursor<=to_add.write_cursor-8){
				addByte(to_add.readByte());
			}
			
			while(!to_add.reachedEnd()){
				addBit(to_add.readBit());
			}
			to_add.read_cursor=last;
		}


		bool readBit(){
			int pw=(1<<(read_cursor%8));
			bool to_return=data[read_cursor/8]&pw;
			read_cursor++;
			return to_return;
		}
		uint8_t readByte(){
			uint8_t to_return=0;
			if(read_cursor%8==0){
				to_return=data[read_cursor/8];
			}
			else{

				int c=read_cursor%8;
				uint8_t bytel=data[read_cursor/8];
				uint8_t byter=data[read_cursor/8+1];
				uint8_t l=bytel>>c;
				uint8_t r=byter<<(8-c);

				to_return=l|r;
			}
			read_cursor+=8;
			return to_return;

		}
	};

	unsigned int bitsToBytes(unsigned int numbits);
	void addInt(uint32_t to_add,uint8_t*data); 
	void addBuffer(const buffer&buff,uint8_t*data);
	unsigned int readInt(const uint8_t*data);
	buffer readBuffer(const uint8_t*data,int numbits);

	Status compress(const uint8_t*data,int size,uint8_t*compress_buffer,int&buffer_size);
	Status decompress(const uint8_t*data,int size,uint8_t*decompress_buffer,int&buffer_size);
	unsigned int getUncompressedSize(const uint8_t*compressed);

	node* createTree(const uint8_t*bytes,int size);
	void assignPaths(node*here,buffer*paths,std::string curr);
	void decodeHuffman(buffer&code,uint8_t*destination,node*here);
	void saveTree(node*here,buffer&tree_data);
	node*loadTree(buffer&tree_data);
}

#endif
