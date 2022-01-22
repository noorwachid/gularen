cmake_minimum_required(VERSION 3.14)

project(Gularen)

set(CMAKE_CXX_STANDARD 17)

# MainLibrary
add_library(Gularen
    src/Gularen/IO.cpp
    src/Gularen/TokenType.cpp
    src/Gularen/Token.cpp
    src/Gularen/Lexer.cpp
    src/Gularen/NodeType.cpp
    src/Gularen/NodeGroup.cpp
    src/Gularen/Node.cpp
    src/Gularen/AstBuilder.cpp)
target_include_directories(Gularen PUBLIC src)

# BridgeLibrary
add_library(GularenBridge
    src/GularenBridge/Html/Renderer.cpp
    src/GularenBridge/Json/Renderer.cpp)
target_include_directories(GularenBridge PUBLIC src)
target_link_libraries(GularenBridge Gularen)

# ConsoleInterface
add_executable(GularenConsole src/ConsoleInterface.hpp src/main.cpp)
target_link_libraries(GularenConsole GularenBridge)
set_target_properties(GularenConsole PROPERTIES OUTPUT_NAME gularen)
