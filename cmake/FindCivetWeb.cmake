find_path(CIVETWEB_INCLUDE_DIR
  NAMES civetweb.h
  DOC "The CivetWeb include directory"
)

find_path(CIVETWEB_CXX_INCLUDE_DIR
  NAMES CivetServer.h
  DOC "The CivetWeb C++ include directory"
)

find_library(CIVETWEB_LIBRARY
  NAMES civetweb
  DOC "The CivetWeb library"
)

find_library(CIVETWEB_CXX_LIBRARY
  NAMES civetweb-cpp
  DOC "The CivetWeb C++ library"
)

mark_as_advanced(CIVETWEB_LIBRARY CIVETWEB_CXX_LIBRARY CIVETWEB_INCLUDE_DIR CIVETWEB_CXX_INCLUDE_DIR)

if(CIVETWEB_INCLUDE_DIR AND EXISTS "${CIVETWEB_INCLUDE_DIR}/civetweb.h")
  file(STRINGS "${CIVETWEB_INCLUDE_DIR}/civetweb.h" civetweb_version_str REGEX "^#define[\t ]+CIVETWEB_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^.*CIVETWEB_VERSION[\t ]+\"([^\"]*)\".*$" "\\1" CIVETWEB_VERSION_STRING "${civetweb_version_str}")
  unset(civetweb_version_str)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CivetWeb
                                  FOUND_VAR CivetWeb_FOUND
                                  REQUIRED_VARS CIVETWEB_LIBRARY CIVETWEB_CXX_LIBRARY CIVETWEB_INCLUDE_DIR CIVETWEB_CXX_INCLUDE_DIR
                                  VERSION_VAR CIVETWEB_VERSION_STRING)

if(CivetWeb_FOUND)
  set(CIVETWEB_LIBRARIES CivetWeb::CivetWeb CivetWeb::CivetWebCpp)
  set(CIVETWEB_INCLUDE_DIRS ${CIVETWEB_INCLUDE_DIR} ${CIVETWEB_CXX_INCLUDE_DIR})
  if(NOT TARGET CivetWeb::CivetWeb)
    add_library(CivetWeb::CivetWeb UNKNOWN IMPORTED)
    set_target_properties(CivetWeb::CivetWeb PROPERTIES
      IMPORTED_LOCATION "${CIVETWEB_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${CIVETWEB_INCLUDE_DIR}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    )
  endif()
  if(NOT TARGET CivetWeb::CivetWebCpp)
    add_library(CivetWeb::CivetWebCpp UNKNOWN IMPORTED)
    set_target_properties(CivetWeb::CivetWebCpp PROPERTIES
      IMPORTED_LOCATION "${CIVETWEB_CXX_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${CIVETWEB_CXX_INCLUDE_DIR}"
      IMPORTED_LINK_INTERFACE_LIBRARIES "CivetWeb::CivetWeb"
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    )
  endif()
endif()
