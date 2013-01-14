#
# Tests
#
enable_testing()

find_package(Gtest REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif()

file(GLOB TEST_SRC_FILES *.cpp)
include_directories(${GTEST_INCLUDE_DIR})
add_executable(runAllTests ${TEST_SRC_FILES})
target_link_libraries(runAllTests duke_core ${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
add_custom_command(TARGET runAllTests POST_BUILD COMMAND runAllTests)
