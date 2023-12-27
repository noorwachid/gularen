em++ --bind \
	-std=c++17 \
	-O3 \
	-Isource \
	-s NO_FILESYSTEM=1 \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp \
	source/Gularen/Internal/Helper/quote.cpp \
	source/Gularen/Internal/Helper/emoji.cpp \
	source/Gularen/Internal/Token.cpp \
	source/Gularen/Internal/Lexer.cpp \
	source/Gularen/Internal/parser.cpp \
	source/Gularen/Node.cpp \
	source/Gularen/parse.cpp \
	source/Gularen/transpile.cpp
