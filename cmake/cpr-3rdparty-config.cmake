if(NOT CMAKE_VERSION VERSION_LESS 3)
  include(CMakeFindDependencyMacro)
  find_dependency(CURL)
endif()

get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/cpr/" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

set_and_check(CPR_INCLUDE_DIR ${_IMPORT_PREFIX}/include)
set(CPR_INCLUDE_DIRS "${CPR_INCLUDE_DIR}")

add_library(cpr OBJECT
  ${_IMPORT_PREFIX}/cpr/auth.cpp
  ${_IMPORT_PREFIX}/cpr/cookies.cpp
  ${_IMPORT_PREFIX}/cpr/cprtypes.cpp
  ${_IMPORT_PREFIX}/cpr/digest.cpp
  ${_IMPORT_PREFIX}/cpr/error.cpp
  ${_IMPORT_PREFIX}/cpr/multipart.cpp
  ${_IMPORT_PREFIX}/cpr/parameters.cpp
  ${_IMPORT_PREFIX}/cpr/payload.cpp
  ${_IMPORT_PREFIX}/cpr/proxies.cpp
  ${_IMPORT_PREFIX}/cpr/session.cpp
  ${_IMPORT_PREFIX}/cpr/ssl_options.cpp
  ${_IMPORT_PREFIX}/cpr/timeout.cpp
  ${_IMPORT_PREFIX}/cpr/util.cpp

  ${_IMPORT_PREFIX}/include/cpr/api.h
  ${_IMPORT_PREFIX}/include/cpr/auth.h
  ${_IMPORT_PREFIX}/include/cpr/body.h
  ${_IMPORT_PREFIX}/include/cpr/cookies.h
  ${_IMPORT_PREFIX}/include/cpr/cpr.h
  ${_IMPORT_PREFIX}/include/cpr/cprtypes.h
  ${_IMPORT_PREFIX}/include/cpr/curlholder.h
  ${_IMPORT_PREFIX}/include/cpr/defines.h
  ${_IMPORT_PREFIX}/include/cpr/digest.h
  ${_IMPORT_PREFIX}/include/cpr/error.h
  ${_IMPORT_PREFIX}/include/cpr/low_speed.h
  ${_IMPORT_PREFIX}/include/cpr/max_redirects.h
  ${_IMPORT_PREFIX}/include/cpr/multipart.h
  ${_IMPORT_PREFIX}/include/cpr/parameters.h
  ${_IMPORT_PREFIX}/include/cpr/payload.h
  ${_IMPORT_PREFIX}/include/cpr/proxies.h
  ${_IMPORT_PREFIX}/include/cpr/response.h
  ${_IMPORT_PREFIX}/include/cpr/session.h
  ${_IMPORT_PREFIX}/include/cpr/ssl_options.h
  ${_IMPORT_PREFIX}/include/cpr/timeout.h
  ${_IMPORT_PREFIX}/include/cpr/util.h
)

target_include_directories(cpr PUBLIC ${CPR_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

if(BUILD_SHARED_LIBS)
  set_target_properties(cpr PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
  )
endif()
