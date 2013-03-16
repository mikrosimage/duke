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
	"${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h"
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE DUKE_PLUGINS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} imageio_plugins/*.c*)
file(GLOB_RECURSE DUKE_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

list(REMOVE_ITEM DUKE_SRC_FILES main.cpp ${DUKE_PLUGINS_FILES})
add_library(duke_core ${DUKE_SRC_FILES} ${PROJECT_SOURCE_DIR}/dependencies/light_sequence_parser/src/FolderParser.cpp)
target_link_libraries(duke_core ${CMAKE_THREAD_LIBS_INIT} glfw ${GLFW_LIBRARIES})

add_executable(duke ${DUKE_PLUGINS_FILES} main.cpp)
target_link_libraries(duke duke_core)

#
# OpenImageIO
#
find_package(OpenImageIO)
if(OPENIMAGEIO_FOUND)
  add_definitions(-DDUKE_OIIO)
  include_directories(${OPENIMAGEIO_INCLUDE_DIRS})
  target_link_libraries(duke ${OPENIMAGEIO_LIBRARIES})
endif(OPENIMAGEIO_FOUND)
