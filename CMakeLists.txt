cmake_minimum_required(VERSION 2.6)
project(RectangularScintillator)

add_subdirectory(math_h)
include_directories(${MATH_H_INC})
get_directory_property(has_parent PARENT_DIRECTORY)
if(has_parent)
	set(RECTSCIN_INC 
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${MATH_H_INC}
		PARENT_SCOPE)
endif()

file(GLOB SOURCES src/*.cpp)
file(GLOB TESTS tests/*.cpp)
file(GLOB EXAMPLES Example/*.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions(-std=c++11 -Wall -Wunused-parameter)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${MATH_H_INC})
add_library(rectscin SHARED ${SOURCES})

foreach(example ${EXAMPLES})
	get_filename_component(ex_app ${example} NAME_WE)
	add_executable(${ex_app} ${example})
	target_link_libraries(${ex_app} rectscin)
endforeach()

option(tests "Build all tests." OFF) 
if(tests)
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
	add_executable(rectscin-test.exe ${TESTS})
	target_link_libraries(rectscin-test.exe rectscin ${GTEST_BOTH_LIBRARIES})
endif()
