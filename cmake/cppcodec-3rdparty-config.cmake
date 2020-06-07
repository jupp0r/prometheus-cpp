get_filename_component(_IMPORT_PREFIX "${PROJECT_SOURCE_DIR}/3rdparty/cppcodec/" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

set_and_check(CPPCODEC_INCLUDE_DIR ${_IMPORT_PREFIX})
set(CPPCODEC_INCLUDE_DIRS "${CPPCODEC_INCLUDE_DIR}")

add_library(cppcodec INTERFACE)
target_include_directories(cppcodec INTERFACE "$<BUILD_INTERFACE:${CPPCODEC_INCLUDE_DIR}>")
