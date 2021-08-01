cmake_minimum_required(VERSION 3.14)

project(Gularen)

set(CMAKE_CXX_STANDARD 17)

add_library(Gularen
    Source/Gularen/IO.cpp
    Source/Gularen/TokenType.cpp
    Source/Gularen/Token.cpp
    Source/Gularen/Lexer.cpp
    Source/Gularen/NodeType.cpp
    Source/Gularen/Node.cpp
    Source/Gularen/AstBuilder.cpp)

add_executable(grr Source/Parser.cpp)
target_link_libraries(grr Gularen)
