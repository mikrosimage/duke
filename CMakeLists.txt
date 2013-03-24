cmake_minimum_required(VERSION 2.8)
project(duke)
# add specific modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

# defining build 
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC DUKE_GIT_SHA1)

set(DUKE_VERSION_MAJOR "2")
set(DUKE_VERSION_MINOR "0")
set(DUKE_VERSION_PATCH "0")
set(DUKE_VERSION_EXTRA "-M1")
set(DUKE_VERSION "${DUKE_VERSION_MAJOR}.${DUKE_VERSION_MINOR}")
set(DUKE_VERSION_FULL "${DUKE_VERSION}.${DUKE_VERSION_PATCH}${DUKE_VERSION_EXTRA}")

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

# clang && macosx
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") AND APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# c++11
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# duke
add_subdirectory(src/duke)

# tests
if(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/gtest-1.6.0/")
add_subdirectory(dependencies/gtest-1.6.0)
add_subdirectory(test)
else()
MESSAGE(STATUS "Could NOT find gtest (missing: 'dependencies/gtest-1.6.0/' folder) : tests disabled
To unable tests
* download https://googletest.googlecode.com/files/gtest-1.6.0.zip
* unzip into 'dependencies/gtest-1.6.0/'")
endif()
