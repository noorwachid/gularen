cmake_minimum_required(VERSION 3.18)

project(gularen
	VERSION 1.0 
	LANGUAGES CXX
)

add_library(gularen 
	source/gularen/lexer.cpp
	source/gularen/token.cpp
	source/gularen/node.cpp
	source/gularen/parser.cpp
	source/gularen/helper/escape.cpp
	source/gularen/helper/emoji.cpp
	source/gularen/transpiler/html.cpp
)

target_include_directories(gularen PUBLIC source)
set_target_properties(gularen PROPERTIES CXX_STANDARD 17)

add_executable(gularen-cli
	source/main.cpp
)

set_target_properties(gularen-cli PROPERTIES CXX_STANDARD 17 OUTPUT_NAME gularen)
target_link_libraries(gularen-cli gularen)
