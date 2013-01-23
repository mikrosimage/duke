#
# OpenGL
#
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
add_definitions(-DGL_GLEXT_PROTOTYPES -DGL3_PROTOTYPES)


#
# GLFW
#
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
add_definitions(-DGLFW_NO_GLU -DGLFW_INCLUDE_GL3)


#
# duke sources
#
file(GLOB_RECURSE DUKE_PLUGINS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} imageio_plugins/*.cpp)
file(GLOB_RECURSE DUKE_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

list(REMOVE_ITEM DUKE_SRC_FILES main.cpp ${DUKE_PLUGINS_FILES})
add_library(duke_core ${DUKE_SRC_FILES} ${PROJECT_SOURCE_DIR}/dependencies/light_sequence_parser/src/FolderParser.cpp)

add_executable(duke ${DUKE_PLUGINS_FILES} main.cpp)
target_link_libraries(duke duke_core ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${GLFW_LIBRARY})


#
# OpenImageIO
#
find_package(OpenImageIO)
if(OPENIMAGEIO_FOUND)
  add_definitions(-DDUKE_OIIO)
  include_directories(${OPENIMAGEIO_INCLUDE_DIRS})
  target_link_libraries(duke ${OPENIMAGEIO_LIBRARIES})
endif(OPENIMAGEIO_FOUND)