cmake_minimum_required(VERSION 2.8)
project(duke)

# add specific modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

#
find_package(Threads)

# adding glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Do not build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Do not build the GLFW tests programs"   FORCE)
add_subdirectory(dependencies/glfw)

#
include_directories(dependencies/concurrent_utils)
include_directories(dependencies/light_sequence_parser/include)
include_directories(SYSTEM dependencies/glfw/include)
include_directories(SYSTEM dependencies/glm)
include_directories(src)

# c++11 / no-rtti
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")
endif()

# duke
add_subdirectory(src/duke)

# tests
find_package(Gtest)
if(GTEST_FOUND)
add_subdirectory(test)
endif()
