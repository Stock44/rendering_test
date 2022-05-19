# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.

cmake_minimum_required(VERSION 3.0)
project(rapidxml VERSION 1.13)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(rapidxml rapidxml.hpp)
set_target_properties(rapidxml PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(rapidxml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})