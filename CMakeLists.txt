cmake_minimum_required(VERSION 3.18)

project("Gularen"
	VERSION 1.0 
	LANGUAGES CXX
)

file(GLOB GULAREN_SRC 
	src/*.h
	src/*.cpp 
	src/Gularen/*.h
	src/Gularen/*.cpp
)

add_library(${PROJECT_NAME} ${GULAREN_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC src)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gularen")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)


if(DEFINED RUN_TEST)
	message("Runing Test")

	add_executable(${PROJECT_NAME}Test test/main.cpp test/Tester.h test/Tester.cpp)

	set_target_properties(${PROJECT_NAME}Test PROPERTIES OUTPUT_NAME "gularen_test")
	set_target_properties(${PROJECT_NAME}Test PROPERTIES CXX_STANDARD 17)


	target_link_libraries(${PROJECT_NAME}Test ${PROJECT_NAME})

endif(DEFINED RUN_TEST)
