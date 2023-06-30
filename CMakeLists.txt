cmake_minimum_required(VERSION 3.18)

project(gularen
	VERSION 1.0 
	LANGUAGES CXX
)

file(GLOB gularen_sources
	source/Gularen/Lexer.cpp
	source/Gularen/Node.cpp
	source/Gularen/Parser.cpp
)

add_library(gularen ${gularen_sources})

target_include_directories(gularen PUBLIC source)
set_target_properties(gularen PROPERTIES CXX_STANDARD 17)

add_executable(gularen-test source/test.cpp)
set_target_properties(gularen-test PROPERTIES CXX_STANDARD 17)
target_link_libraries(gularen-test gularen)
