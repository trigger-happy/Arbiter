cmake_minimum_required(VERSION 2.6)
project("Arbiter")

SET(Boost_USE_MULTITHREAD ON)
SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_ADDITIONAL_VERSIONS "1.43.0")
FIND_PACKAGE(Boost 1.44.0 REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
