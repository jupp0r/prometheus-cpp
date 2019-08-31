
add_library(core
  src/check_names.cc
  src/counter.cc
  src/detail/builder.impl.h
  src/detail/ckms_quantiles.cc
  src/detail/time_window_quantiles.cc
  src/detail/utils.cc
  src/gauge.cc
  src/family.impl.h
  src/histogram.cc
  src/registry.cc
  src/registry.impl.h
  src/serializer.cc
  src/summary.cc
  src/text_serializer.cc
)

add_library(${PROJECT_NAME}::core ALIAS core)

generate_export_header(core
  EXPORT_FILE_NAME include/prometheus/detail/core_export.h
  PREFIX_NAME PROMETHEUS_CPP_
)

target_link_libraries(core
  PRIVATE
    Threads::Threads
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
)

target_include_directories(core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

set_target_properties(core
  PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}-core
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

install(
  TARGETS core
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()

if(benchmark_FOUND)
  add_subdirectory(benchmarks)
endif()
