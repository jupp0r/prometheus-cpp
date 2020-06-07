find_path(CPPCODEC_INCLUDE_DIR base64_rfc4648.hpp PATH_SUFFIXES include/cppcodec)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cppcodec DEFAULT_MSG CPPCODEC_INCLUDE_DIR)

if(cppcodec_FOUND)
  set(CPPCODEC_INCLUDE_DIRS "${CPPCODEC_INCLUDE_DIR}")
endif()
