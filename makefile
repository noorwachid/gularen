build:
	g++ -std=c++17 -O2 -o gularen source/*.cpp source/Collection/*.cpp

install: build
	mv gularen /usr/local/bin/gularen
	
