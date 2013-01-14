# - Find GLFW library
# Find the native GLFW includes and library
# This module defines
#  GLFW_INCLUDE_DIR, where to find glfw.h.
#  GLFW_LIBRARY, libraries to link against to use GLFW.
#  GLFW_ROOT_DIR, The base directory to search for GLFW.
#                        This can also be an environment variable.
#  GLFW_FOUND, If false, do not try to use GLFW.
#

# If GLFW_ROOT_DIR was defined in the environment, use it.
IF(NOT GLFW_ROOT_DIR AND NOT $ENV{GLFW_ROOT_DIR} STREQUAL "")
  SET(GLFW_ROOT_DIR $ENV{GLFW_ROOT_DIR})
ENDIF()

SET(_glfw_SEARCH_DIRS
  ${GLFW_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

FIND_PATH(GLFW_INCLUDE_DIR
  NAMES         GL/glfw.h
  HINTS         ${_glfw_SEARCH_DIRS}
  PATH_SUFFIXES include
)

FIND_LIBRARY(GLFW_LIBRARY
  NAMES         glfw
  HINTS         ${_glfw_SEARCH_DIRS}
  PATH_SUFFIXES lib64 lib
  )

# handle the QUIETLY and REQUIRED arguments and set GLFW_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

IF(GLFW_FOUND)
  SET(GLFW_LIBRARY ${GLFW_LIBRARY})
  SET(GLFW_INCLUDE_DIR ${GLFW_INCLUDE_DIR})
ENDIF(GLFW_FOUND)

MARK_AS_ADVANCED(
  GLFW_INCLUDE_DIR
  GLFW_LIBRARY
)
