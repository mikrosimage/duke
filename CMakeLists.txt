cmake_minimum_required(VERSION 2.8)
project(duke)
# add specific modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

# defining build 
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC DUKE_GIT_SHA1)
git_describe(DUKE_VERSION --all)

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

# clang && debug adds address sanitizer
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") AND NOT APPLE)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()

# duke
add_subdirectory(src/duke)

# tests
add_subdirectory(third_party/gtest-1.7.0)
add_subdirectory(test)
