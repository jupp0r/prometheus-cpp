add_library(prometheus-cpp-core
  src/check_names.cc
  src/counter.cc
  src/counter_builder.cc
  src/gauge.cc
  src/gauge_builder.cc
  src/histogram.cc
  src/histogram_builder.cc
  src/registry.cc
  src/serializer.cc
  src/summary.cc
  src/summary_builder.cc
  src/text_serializer.cc
)

set_target_properties(prometheus-cpp-core PROPERTIES EXPORT_NAME core)

target_link_libraries(prometheus-cpp-core PRIVATE ${CMAKE_THREAD_LIBS_INIT})
if(UNIX AND NOT APPLE)
  target_link_libraries(prometheus-cpp-core PRIVATE rt)
endif()

target_include_directories(prometheus-cpp-core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS prometheus-cpp-core EXPORT prometheus-cpp-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
