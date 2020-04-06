#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<vector>
#include<queue>
#include<stdio.h>
#include<string.h>
#include<iostream>

namespace Huffman{
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



	void addBuffer(const buffer&buff,uint8_t*data);
	buffer readBuffer(const uint8_t*data);

	std::vector<uint8_t>  compress(const uint8_t*data,int size);
	std::vector<uint8_t> decompress(const uint8_t*data);

	node* createTree(const std::vector<uint8_t>&bytes);
	void assignPaths(node*here,std::string*paths,std::string curr);
	void addToBuffer(buffer&buff,const std::string&to_add);
	void decodeHuffman(buffer&code,std::vector<uint8_t>&destination,node*here);
	void saveTree(node*here,buffer&tree_data);
	node*loadTree(buffer&tree_data);
}

#endif
