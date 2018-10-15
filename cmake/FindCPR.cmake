find_path(CPR_INCLUDE_DIR
  NAMES cpr/cpr.h
  DOC "The CPR include directory"
)

find_library(CPR_LIBRARY
  NAMES cpr
  DOC "The CPR library"
)

mark_as_advanced(CPR_LIBRARY CPR_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPR REQUIRED_VARS CPR_LIBRARY CPR_INCLUDE_DIR)

if(CPR_FOUND)
  set(CPR_LIBRARIES CPR::CPR)
  set(CPR_INCLUDE_DIRS ${CPR_INCLUDE_DIR})
  if(NOT TARGET CPR::CPR)
    add_library(CPR::CPR UNKNOWN IMPORTED)
    set_target_properties(CPR::CPR PROPERTIES
      IMPORTED_LOCATION "${CPR_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${CPR_INCLUDE_DIR}"
      IMPORTED_LINK_INTERFACE_LIBRARIES "${CURL_LIBRARIES}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    )
  endif()
endif()
