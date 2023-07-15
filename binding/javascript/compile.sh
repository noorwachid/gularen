em++ --bind  -std=c++17 -O3 -Isource \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp \
	source/Gularen/Helper/escape.cpp \
	source/Gularen/Token.cpp \
	source/Gularen/Lexer.cpp \
	source/Gularen/Node.cpp \
	source/Gularen/Parser.cpp \
	source/Gularen/Transpiler/HTML.cpp
