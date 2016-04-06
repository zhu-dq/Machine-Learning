cmake_minimum_required(VERSION 3.5)
project(CorrelationAnalysis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp CorrelationAnalysis.cpp CorrelationAnalysis.h CA_Utils.cpp CA_Utils.h)
add_executable(CorrelationAnalysis ${SOURCE_FILES})