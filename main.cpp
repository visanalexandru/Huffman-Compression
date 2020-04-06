#include<iostream>
#include "Huffman.h"
#include<fstream>
#include<sstream>
using namespace std;


int main(){
	
	double a =clock();
	ifstream in("input");

	stringstream buffer;
    buffer << in.rdbuf();
	string to_compress=buffer.str();
	

	vector<uint8_t> raw;
	raw.resize(to_compress.length());
	memcpy(&raw[0],&to_compress[0],to_compress.length());



	vector<uint8_t> compressed=Huffman::compress(&raw[0],to_compress.length());
	cout<<(clock()-a)/CLOCKS_PER_SEC<<std::endl;

	a=clock();
	std::cout<<compressed.size()<<std::endl;
	vector<uint8_t> decompressed=Huffman::decompress(&compressed[0]);
	cout<<(clock()-a)/CLOCKS_PER_SEC<<std::endl;
	if(decompressed==raw)
		std::cout<<"YES";

	return 0;
}
