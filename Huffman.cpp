#include "Huffman.h"

namespace Huffman{


	void addInt(uint32_t to_add,uint8_t*data){
		memcpy(data,&to_add,sizeof(to_add));
	}

	void addBuffer(const buffer&buff,uint8_t*data){
		memcpy(data,&buff.data[0],buff.data.size());	
	}

	uint32_t readInt(const uint8_t*data){
		uint32_t to_return;
		memcpy(&to_return,data,sizeof(to_return));
		return to_return;
	}

	unsigned int bitsToBytes(unsigned int numbits){
		unsigned int to_return=numbits/8;
		if(numbits%8)
			to_return++;
		return to_return;
	}
	
	buffer readBuffer(const uint8_t*data,int numbits){

		int bytes_to_read=bitsToBytes(numbits);

		buffer to_return;
		to_return.data.resize(bytes_to_read);

		memcpy(&to_return.data[0],data,bytes_to_read);

		to_return.write_cursor=numbits;
		return to_return;
	}

	Status compress(const uint8_t*data,int size,uint8_t*compress_buffer,int&buffer_size){
		std::vector<uint8_t> cpy;



		cpy.resize(size);
		memcpy(&cpy[0],data,size);


		buffer paths[256];
		node*root=createTree(cpy);

		if(!root->isLeaf){
			assignPaths(root,paths,"");
		}
		else {
			paths[(int)cpy[0]]=buffer("0");	
		}

		buffer compressed,tree;

		for(const uint8_t&a:cpy){
			compressed.addBuffer(paths[(int)a]);
		}

		saveTree(root,tree);
		int total_size=compressed.data.size()+tree.data.size()+2*sizeof(uint32_t);		
		if(buffer_size<total_size)
			return Status::BufferTooSmall;

		buffer_size=total_size;
		addInt(compressed.write_cursor,compress_buffer);
		addInt(tree.write_cursor,compress_buffer+sizeof(uint32_t));

		addBuffer(compressed,compress_buffer+2*sizeof(uint32_t));
		addBuffer(tree,compress_buffer+2*sizeof(uint32_t)+compressed.data.size());



		delete root;
		return Status::Done;
	}
	Status decompress(const uint8_t*data,int size,uint8_t*decompress_buffer,int&buffer_size){
	
		if(size<2*sizeof(uint32_t))
			return Status::InvalidInput;

		unsigned int comp_bits=readInt(data);
		unsigned int tree_bits=readInt(data+sizeof(uint32_t));

		unsigned int comp_size=bitsToBytes(comp_bits);
		unsigned int tree_size=bitsToBytes(tree_bits);

		buffer compressed=readBuffer(data+2*sizeof(uint32_t),comp_bits);
		buffer tree=readBuffer(data+2*sizeof(uint32_t)+comp_size,tree_bits);	



		node*root=loadTree(tree);
		decodeHuffman(compressed,decompress_buffer,root);
		delete root;

		return Status::Done;
	}


	node*createTree(const std::vector<uint8_t>&bytes){
		int frequency[256]{};
		std::priority_queue<node*,std::vector<node*>,heapCompare> heap;

		for(uint8_t a:bytes){
			frequency[(int)a]++;
		}

		for(int i=0;i<256;i++){

			if(frequency[i]){
				heap.push(new node((uint8_t)i,frequency[i]));
			}
		}

		while(heap.size()>1){
			node*a=heap.top();
			heap.pop();
			node*b=heap.top();
			heap.pop();


			node* newnode=new node(a,b);
			heap.push(newnode); 

		}

		return heap.top();
	}

	node*loadTree(buffer&tree_data){
		bool newbit=tree_data.readBit();

		if(newbit){
			return new node(tree_data.readByte(),0);
		}
		else{
			return new node(loadTree(tree_data),loadTree(tree_data));
		}
	}

	void saveTree(node*here,buffer&tree_data){
		if(here->isLeaf){
			tree_data.addBit(true);
			tree_data.addByte(here->data);
		}
		else{
			tree_data.addBit(false);
			saveTree(here->left,tree_data);
			saveTree(here->right,tree_data);
		}
	}

	void assignPaths(node*here,buffer*paths,std::string curr){
		if(here->isLeaf){
			paths[(int)(here->data)]=buffer(curr);
		}
		else{
			assignPaths(here->left,paths,curr+'0');
			assignPaths(here->right,paths,curr+'1');
		}
	}

	void decodeHuffman(buffer&code,uint8_t*destination,node*root){

		node*here=root;
		int cursor=0;
		if(here->isLeaf){
			for(int i=0;i<code.write_cursor;i++)
				destination[i]=here->data;
		}
		else{

			while(!code.reachedEnd()){
				bool bit=code.readBit();
				if(!bit){
					here=here->left;
				}
				else here=here->right;

				if(here->isLeaf){
					destination[cursor]=here->data;
					cursor++;
					here=root;
				}
			}
		}

	}
}
