#ifndef HUFFMAN_INTERNAL_H
#define HUFFMAN_INTERNAL_H
#include<vector>
#include<queue>
#include<stdio.h>
#include<string.h>
#include<iostream>


namespace HuffmanInternal{


	//The huffman binary tree is built using this structure
	struct node{
		node*left,*right;
		uint8_t data;
		int cost;
		bool isLeaf;

		//is a leaf node
		node(uint8_t byte,int value):left(nullptr),
		right(nullptr),
		data(byte),
		cost(value),
		isLeaf(true)
		{

		}
		//If it is a parent node, its cost will be equal to the sum its children
		node(node*l,node*r):left(l),
		right(r),
		data(' '),
		cost(l->cost+r->cost),
		isLeaf(false){

		}

		//delete the children
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


	/* This structure is a buffer than supports addition of individual bits. This is useful because each 
	 * huffman code can be smaller than a byte.
	 */
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

	//Copies the integer into the output buffer
	void addInt(uint32_t to_add,uint8_t*data);
	//Copies the buffer into the output buffer
	void addBuffer(const buffer&buff,uint8_t*data);

	//Reads an integer from the input buffer
	unsigned int readInt(const uint8_t*data);

	//Reads a buffer from the input buffer, and sets its writing cursor accordingly
	buffer readBuffer(const uint8_t*data,int numbits);
	//How many bytes are required to hold given bit count
	unsigned int bitsToBytes(unsigned int numbits);

	//Creates huffman tree from input data
	node*createTree(const uint8_t*bytes,int size);	
	//Loads a huffman tree from a buffer
	node*loadTree(buffer&tree_data);
	//Saves a huffman tree to a buffer
	void saveTree(node*here,buffer&tree_data);

	//Creates the prefix codes for every node in the huffman tree
	void assignPaths(node*here,buffer*paths,std::string curr);

	//Decodes the huffman code contained in the buffer
	void decodeHuffman(buffer&code,uint8_t*destination,node*root);

}

#endif
