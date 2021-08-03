cmake_minimum_required(VERSION 3.14)

project(Gularen)

set(CMAKE_CXX_STANDARD 17)

add_library(Gularen
    Source/Gularen/IO.cpp
    Source/Gularen/TokenType.cpp
    Source/Gularen/Token.cpp
    Source/Gularen/Lexer.cpp
    Source/Gularen/NodeType.cpp
    Source/Gularen/NodeGroup.cpp
    Source/Gularen/Node.cpp
    Source/Gularen/AstBuilder.cpp)
target_include_directories(Gularen PUBLIC Source)

add_library(GularenBridge
    Source/GularenBridge/Html5/Renderer.cpp)
target_include_directories(GularenBridge PUBLIC Source)
target_link_libraries(GularenBridge Gularen)

add_executable(grparser Source/Parser.cpp)
target_link_libraries(grparser GularenBridge)
