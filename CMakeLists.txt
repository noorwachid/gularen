cmake_minimum_required(VERSION 3.14)

project(Gularen)

set(CMAKE_CXX_STANDARD 17)

# MainLibrary
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

# BridgeLibrary
add_library(GularenBridge
    Source/GularenBridge/Html5/Renderer.cpp
    Source/GularenBridge/Json/Renderer.cpp)
target_include_directories(GularenBridge PUBLIC Source)
target_link_libraries(GularenBridge Gularen)

# ConsoleInterface
add_executable(gularen Source/ConsoleInterface.cpp)
target_link_libraries(gularen GularenBridge)
