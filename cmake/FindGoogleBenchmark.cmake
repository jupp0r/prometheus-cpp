find_library(GoogleBenchmark_LIBRARY NAMES benchmark)
find_path(GoogleBenchmark_INCLUDE_DIR benchmark/benchmark.h)
mark_as_advanced(GoogleBenchmark_LIBRARY GoogleBenchmark_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GoogleBenchmark
  FOUND_VAR GoogleBenchmark_FOUND
  REQUIRED_VARS
    GoogleBenchmark_LIBRARY
    GoogleBenchmark_INCLUDE_DIR
)

if(GoogleBenchmark_FOUND)
  set(GoogleBenchmark_LIBRARIES ${GoogleBenchmark_LIBRARY})
  set(GoogleBenchmark_INCLUDE_DIRS ${GoogleBenchmark_INCLUDE_DIR})

  if(NOT TARGET Google::Benchmark)
    add_library(Google::Benchmark UNKNOWN IMPORTED)
    set_target_properties(Google::Benchmark PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${GoogleBenchmark_INCLUDE_DIR}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${GoogleBenchmark_LIBRARY}"
    )
  endif()
endif()
