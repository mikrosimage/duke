cmake_minimum_required(VERSION 2.8)
project(duke)

find_package(OpenGL REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fno-rtti")
endif()

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(./dependencies/concurrent_utils)
include_directories(./dependencies/glfw/include)
include_directories(./dependencies/glm)
include_directories(./dependencies/light_sequence_parser/include)

add_subdirectory(src)

# # set(TEST_ENABLED false)
# # if(TEST_ENABLED)
# # enable_testing()
# # add_subdirectory(test)
# endif(TEST_ENABLED)