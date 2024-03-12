C_INCLUDE_PATH=""
CPLUS_INCLUDE_PATH=""

em++ --bind \
	-std=c++11 \
	-O3 \
	-Isource \
	-s NO_FILESYSTEM=1 \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp
