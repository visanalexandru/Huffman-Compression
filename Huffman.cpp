#include "Huffman.h"

namespace Huffman{


	void addBuffer(const buffer&buff,uint8_t*data){
		uint32_t numbits=buff.write_cursor;

		memcpy(data,&numbits,sizeof(numbits));
		memcpy(data+sizeof(numbits),&buff.data[0],buff.data.size());	
	}



	buffer readBuffer(const uint8_t*data){
		uint32_t numbits;
		memcpy(&numbits,data,sizeof(numbits));

		int bytes_to_read=numbits/8;
		if(numbits%8)
			bytes_to_read++;


		buffer to_return;
		to_return.data.resize(bytes_to_read);

		memcpy(&to_return.data[0],data+sizeof(numbits),bytes_to_read);

		to_return.write_cursor=numbits;


		return to_return;
	}

	std::vector<uint8_t> compress(const uint8_t*data,int size){
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

		std::vector<uint8_t> to_return;
		to_return.resize(compressed.data.size()+tree.data.size()+2*sizeof(uint32_t));

		addBuffer(compressed,&to_return[0]);
		addBuffer(tree,&to_return[sizeof(uint32_t)+compressed.data.size()]);

		return to_return;
	}

	std::vector<uint8_t> decompress(const uint8_t*data){
		buffer compressed=readBuffer(data);
		buffer tree=readBuffer(data+sizeof(uint32_t)+compressed.data.size());	
		
		
		node*root=loadTree(tree);
		std::vector<uint8_t> to_return;

		decodeHuffman(compressed,to_return,root);

		return to_return;

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

	void decodeHuffman(buffer&code,std::vector<uint8_t>&destination,node*root){

		node*here=root;
		if(here->isLeaf){
			destination.insert(destination.end(),code.write_cursor,here->data);
		}
		else{

			while(!code.reachedEnd()){
				bool bit=code.readBit();
				if(!bit){
					here=here->left;
				}
				else here=here->right;

				if(here->isLeaf){
					destination.push_back(here->data);
					here=root;
				}
			}
		}

	}
}
