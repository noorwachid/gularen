cmake_minimum_required(VERSION 3.18)

project(gularen
	VERSION 1.0 
	LANGUAGES CXX
)

file(GLOB gularen_sources
	source/gularen/lexer.cpp
	source/gularen/token.cpp
	source/gularen/node.cpp
	source/gularen/parser.cpp
	source/gularen/helper/escape.cpp
	source/gularen/helper/emoji.cpp
)

add_library(gularen ${gularen_sources})

target_include_directories(gularen PUBLIC source)
set_target_properties(gularen PROPERTIES CXX_STANDARD 17)

add_executable(gularen-test source/gularen/cli/test.cpp)
set_target_properties(gularen-test PROPERTIES CXX_STANDARD 17)
target_link_libraries(gularen-test gularen)

add_executable(gularen-tk
	source/gularen/cli/tk.cpp
	source/gularen/transpiler/html.cpp
)

set_target_properties(gularen-tk PROPERTIES CXX_STANDARD 17)
target_link_libraries(gularen-tk gularen)
