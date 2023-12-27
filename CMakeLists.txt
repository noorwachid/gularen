cmake_minimum_required(VERSION 3.18)

project(gularen
	VERSION 1.0 
	LANGUAGES CXX
)

add_library(gularen 
	source/Gularen/Node.cpp
	source/Gularen/parse.cpp
	source/Gularen/transpile.cpp
	source/Gularen/Internal/Lexer.cpp
	source/Gularen/Internal/Token.cpp
	source/Gularen/Internal/Parser.cpp
	source/Gularen/Internal/Helper/quote.cpp
	source/Gularen/Internal/Helper/emoji.cpp
)

target_include_directories(gularen PUBLIC source)
set_target_properties(gularen PROPERTIES CXX_STANDARD 17)

add_executable(gularen-cli cli/main.cpp)
set_target_properties(gularen-cli PROPERTIES CXX_STANDARD 17 OUTPUT_NAME gularen)
target_link_libraries(gularen-cli gularen)

add_executable(gularen-test test/main.cpp)
set_target_properties(gularen-test PROPERTIES CXX_STANDARD 17)
target_link_libraries(gularen-test gularen)
