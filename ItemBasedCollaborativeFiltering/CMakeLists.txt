cmake_minimum_required(VERSION 3.5)
project(ItemBasedCollaborativeFiltering)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

set(SOURCE_FILES main.cpp item_cfengine.cpp item_cfengine.h utils.cpp utils.h)
add_executable(ItemBasedCollaborativeFiltering ${SOURCE_FILES})