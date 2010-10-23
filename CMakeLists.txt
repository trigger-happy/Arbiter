cmake_minimum_required(VERSION 2.6)
project("Arbiter")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

SET(Boost_USE_MULTITHREAD ON)
SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_ADDITIONAL_VERSIONS "1.43.0")
FIND_PACKAGE(Boost 1.44.0 COMPONENTS program_options 
					system
					thread
					regex
					unit_test_framework
					signals
					iostreams
					REQUIRED)

FIND_PACKAGE(Wt REQUIRED)

FIND_PACKAGE(Apr REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include"
			${Boost_INCLUDE_DIRS}
			${Wt_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)