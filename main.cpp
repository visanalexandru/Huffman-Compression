#include<iostream>
#include "Huffman.h"
#include<fstream>
#include<sstream>
using namespace std;


int main(){




	ifstream in("input");

	stringstream buffer;
    buffer << in.rdbuf();
	string to_compress=buffer.str();

	vector<uint8_t> raw;
	raw.resize(to_compress.length());
	memcpy(&raw[0],&to_compress[0],to_compress.length());


	vector<uint8_t> compressed;
	compressed.resize(raw.size()+332);
	int compressed_size=compressed.size();

	double a =clock();
	Huffman::compress(&raw[0],raw.size(),&compressed[0],compressed_size);
	cout<<(clock()-a)/CLOCKS_PER_SEC<<std::endl;

	
	std::cout<<compressed_size<<std::endl;

	vector<uint8_t> decompressed;
	decompressed.resize(Huffman::getUncompressedSize(&compressed[0]));
	int decompressed_size=decompressed.size();

	a=clock();
	Huffman::decompress(&compressed[0],compressed_size,&decompressed[0],decompressed_size);
	cout<<(clock()-a)/CLOCKS_PER_SEC<<std::endl;
	if(decompressed==raw)
		std::cout<<"YES";

	return 0;
}
