em++ --bind  -std=c++17 -O3 -Isource \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp \
	source/gularen/helper/escape.cpp \
	source/gularen/helper/emoji.cpp \
	source/gularen/token.cpp \
	source/gularen/lexer.cpp \
	source/gularen/node.cpp \
	source/gularen/parser.cpp \
	source/gularen/transpiler/html.cpp
