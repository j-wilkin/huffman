all: hencode hdecode

hencode: hencode.cc
	g++ -o hencode hencode.cc

hdecode: hdecode.cc
	g++ -o hdecode hdecode.cc