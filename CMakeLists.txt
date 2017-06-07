cmake_minimum_required(VERSION 2.6)
project(gdemu)

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

find_package(X11 REQUIRED)
find_package(SDL REQUIRED)

subdirs(src
	examples)
