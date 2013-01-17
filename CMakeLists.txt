cmake_minimum_required(VERSION 2.8)
project(duke)

# add specific modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

#
find_package(Threads)

#
include_directories(dependencies/concurrent_utils)
include_directories(dependencies/glm)
include_directories(dependencies/light_sequence_parser/include)
include_directories(src)

# c++11 / no-rtti
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fno-rtti -g")
endif()

# duke
add_subdirectory(src/duke)

# tests
add_subdirectory(test)
