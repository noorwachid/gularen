em++ --bind  -std=c++17 -Isource \
	-o binding/javascript/gularen.js \
	binding/javascript/binding.cpp \
	source/Gularen/Lexer.cpp \
	source/Gularen/Parser.cpp \
	source/Gularen/Node.cpp \
	source/Gularen/Transpiler/HTML.cpp
