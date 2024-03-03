em++ --bind \
	-std=c++17 \
	-O3 \
	-Isource \
	-s NO_FILESYSTEM=1 \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp
