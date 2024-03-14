C_INCLUDE_PATH=""
CPLUS_INCLUDE_PATH=""

em++ --bind \
	-std=c++11 \
	-Isource \
	-O2 \
	-s NO_FILESYSTEM=1 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp

