# defining some variables
set(THIRD_PARTY_DIST "${PROJECT_SOURCE_DIR}/third_party_dist")
set(THIRD_PARTY_INCLUDE "${THIRD_PARTY_DIST}/include")
set(THIRD_PARTY_LIB "${THIRD_PARTY_DIST}/lib")
set(THIRD_PARTY_BIN "${THIRD_PARTY_DIST}/bin")

# adding Wall to configurations
if (NOT MSVC)
    add_definitions ("-Wall")
endif()

#
# Removing rtti for duke
#
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

#
# OpenGL
#
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
add_definitions(-DGL_GLEXT_PROTOTYPES -DGL3_PROTOTYPES)

#
# GLFW
#
add_definitions(-DGLFW_NO_GLU)
             
FIND_FILE(GL3HEADER gl3.h)
if(NOT ${GL3HEADER} MATCHES NOTFOUND)
add_definitions(-DGLFW_INCLUDE_GL3)
endif()

if(APPLE)
add_definitions(-DGLFW_INCLUDE_GLCOREARB)
endif()

#
# duke sources
#
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

file(GLOB_RECURSE DUKE_PLUGINS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} io_plugins/*.c*)
file(GLOB_RECURSE DUKE_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM DUKE_SRC_FILES main.cpp ${DUKE_PLUGINS_FILES})
add_library(
  duke_core
#  flatbuffers/shaders_generated.h
  ${DUKE_SRC_FILES}
  ${PROJECT_SOURCE_DIR}/dependencies/light_sequence_parser/src/FolderParser.cpp
)

target_link_libraries(duke_core ${CMAKE_THREAD_LIBS_INIT} glfw ${GLFW_LIBRARIES})

#
# flatbuffers generation
#
# create destination folder
#execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers/)
#add_custom_command(
#  OUTPUT flatbuffers/shaders_generated.h
#  COMMAND ${THIRD_PARTY_BIN}/flatc
#  ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers/ -c shaders.fbs
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers
#)

find_package(PkgConfig)

#
# OpenImageIO
#
pkg_check_modules(PC_OIIO REQUIRED OpenImageIO>=1.4.10)
add_definitions(-DDUKE_OIIO)
include_directories(${PC_OIIO_INCLUDE_DIRS})
link_directories(${PC_OIIO_LIBRARY_DIRS})

#
# LibAv
#
pkg_check_modules(PC_LIBAV REQUIRED libavformat=55.12.0 libavcodec=55.34.1 libavutil=53.3.0 libswscale=2.1.2)
add_definitions(-DDUKE_LIBAV)
include_directories(${PC_LIBAV_INCLUDE_DIRS})
link_directories(${PC_LIBAV_LIBRARY_DIRS})

# Fast DPX
add_definitions(-DDUKE_FAST_DPX)

# Duke executable
add_executable(duke ${DUKE_PLUGINS_FILES} main.cpp)

target_link_libraries(duke duke_core)
target_link_libraries(duke ${PC_OIIO_LIBRARIES})
target_link_libraries(duke ${PC_LIBAV_LIBRARIES})