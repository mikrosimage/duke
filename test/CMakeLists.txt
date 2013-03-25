#
# Tests
#
enable_testing()

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
	set(CMAKE_CXX_FLAGS "-Wno-sign-compare ${CMAKE_CXX_FLAGS}")
endif()

add_definitions(-DGL_GLEXT_PROTOTYPES -DGL3_PROTOTYPES)
file(GLOB TEST_SRC_FILES *.cpp)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(runAllTests ${TEST_SRC_FILES})
target_link_libraries(runAllTests duke_core gtest_main gtest)
add_custom_command(TARGET runAllTests POST_BUILD COMMAND runAllTests)