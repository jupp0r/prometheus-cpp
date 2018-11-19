set(sources 
  src/check_names.cc
  src/counter.cc
  src/detail/ckms_quantiles.cc
  src/detail/counter_builder.cc
  src/detail/gauge_builder.cc
  src/detail/histogram_builder.cc
  src/detail/summary_builder.cc
  src/detail/time_window_quantiles.cc
  src/gauge.cc
  src/histogram.cc
  src/registry.cc
  src/serializer.cc
  src/summary.cc
  src/text_serializer.cc
  )

add_library(core STATIC ${sources})

add_library(${PROJECT_NAME}::core ALIAS core)

target_link_libraries(core
  PRIVATE
    Threads::Threads
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
)

target_include_directories(core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(core PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-core)

if (ENABLE_SHARED_BUILD)
add_library(core_shared SHARED ${sources})

add_library(${PROJECT_NAME}::core_shared ALIAS core_shared)

target_link_libraries(core_shared
  PRIVATE
    Threads::Threads
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
)

target_include_directories(core_shared
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(core_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-core)

install(
  TARGETS core_shared
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
endif()


install(
  TARGETS core
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()

if(GoogleBenchmark_FOUND)
  add_subdirectory(benchmarks)
endif()
