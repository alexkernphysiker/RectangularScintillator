cmake_minimum_required(VERSION 2.6)
project(RectangleScintillator)
set(CMAKE_CXX_FLAGS "-pthread -std=gnu++11")
add_library(rectscin SHARED geometry.cpp rectscin.cpp sensitive.cpp)
