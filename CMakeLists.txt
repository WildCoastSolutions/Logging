cmake_minimum_required (VERSION 2.6)

project (Logging)

SET (CMAKE_CXX_COMPILER "/usr/bin/clang++-3.5")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread" )

add_subdirectory (Test)